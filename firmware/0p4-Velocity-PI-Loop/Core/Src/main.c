/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

FDCAN_HandleTypeDef hfdcan1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

volatile uint32_t rst = 0; //reset cue
volatile uint32_t pwm_ms = 5000;  //pwm timeout

volatile float pole_pairs = 20.0f; // pole pairs

volatile uint32_t pwm_run = 0;

volatile uint32_t ol_mode = 0;    /* 1 = open-loop */
volatile float ol_amp  = 0.0f;    /* vector amplitude (0.4 A per count) */
volatile float ol_deg  = 0.0f;    /* electrical angle, degrees */
volatile float ol_rate = 0.0f;    /* degrees added per loop (~1 ms) */
volatile float ol_v = 0.0f;        /* vector amplitude in volts; ~0.1 V ≈ 1 A */

//encoder
volatile uint16_t raw, ang, amin = 0xFFFF, amax = 0;
volatile uint32_t par_err = 0, ef_cnt = 0, reads = 0;
volatile uint32_t drv_on = 1;   /* 1 = DRV awake, 0 = asleep */

//adc
volatile uint32_t nf_adc;
volatile uint32_t adc_st;
volatile uint32_t vref_adc, vref_st, vdda_mv;

//sense outputs
volatile uint16_t so[3], so_min[3] = {4095,4095,4095}, so_max[3] = {0,0,0};
volatile uint16_t vr_min = 4095, vr_max = 0;
volatile uint32_t cal_on = 0;      /* 1 = DRV_CAL on */

//CAN
FDCAN_TxHeaderTypeDef txh;
FDCAN_RxHeaderTypeDef rxh;
uint8_t txd[8], rxd[8];
volatile uint32_t can_tx = 0, can_rx = 0, can_txerr = 0, can_rxid = 0;
volatile uint32_t can_tec = 0, can_rec = 0, can_busoff = 0;

//PWM
volatile uint32_t pwm_on = 0;              /* 1 = enable */
volatile uint32_t duty[3] = {0, 0, 0};     /* percent A,B,C */

//encoder alignment
volatile int32_t  off[3] = {2048, 2048, 2048};   /* zero-current offsets, counts */
volatile float    ia, ib, ic, isum;              /* phase currents, amps */
volatile uint32_t isr_cnt = 0, cal_req = 1;      /* cal_req = 1: measure offsets */

#define A_PER_COUNT  (3.3f / 4095.0f / (40.0f * 0.001f))   /* ~0.0201 A/count */

volatile float    e_deg;            /* electrical angle from encoder, degrees */
volatile float    e_off = 0.0f;     /* alignment offset, degrees */
volatile uint32_t align_req = 0;    /* set to 1 during a hold at ol_deg = 0 */

//foc
volatile uint32_t foc_mode = 0;          /* 1 = current loop drives the PWM */
volatile float id_ref = 0.0f, iq_ref = 0.0f;   /* commanded currents, A */
volatile float id, iq, vd, vq;           /* measured currents, output voltages */
volatile float kp = 0.3f;                /* V/A   (L × ωc, ωc = 2π·1 kHz) */
volatile float ki_ts = 0.03f;            /* V/A per sample (R × ωc × Ts) */
volatile float i_max = 2.0f;             /* command limit, A */
volatile float i_trip = 8.0f;            /* instant shutdown, A */
volatile uint32_t foc_fault = 0, isr_cycles = 0;
#define VBUS 12.0f

volatile float e_err;

//FLASH

#define CAL_ADDR  0x0801F800UL        /* last 2 KB page */
#define CAL_PAGE  63
#define CAL_MAGIC 0xCA1B0001UL
typedef struct __attribute__((aligned(8))) {
    uint32_t magic, ver;
    float    e_off, pole_pairs;
    uint32_t sum, pad;
} cal_t;
volatile uint32_t save_req = 0, cal_ok = 0;
volatile int32_t  save_res = 0;

volatile uint32_t vel_mode = 0;      /* 1 = velocity loop sets iq */
volatile float vel_ref  = 0.0f;      /* commanded speed, rad/s (mechanical) */
volatile float vel      = 0.0f;      /* estimated speed, rad/s */
volatile float rpm      = 0.0f;      /* same, in RPM, for viewing */
volatile float pos      = 0.0f;      /* multi-turn position, rad */
volatile float vel_kp   = 0.05f;     /* A per (rad/s) */
volatile float vel_ki   = 0.5f;      /* A per rad */
volatile float pll_bw   = 1000.0f;   /* speed estimator bandwidth, rad/s */
volatile float vel_trip = 150.0f;    /* overspeed shutdown, rad/s (~1430 RPM) */
volatile float iq_cmd   = 0.0f;      /* iq actually used by the current loop */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI2_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC1_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint32_t cal_sum(const cal_t *c) {
    const uint32_t *p = (const uint32_t *)c;
    return p[0] + p[1] + p[2] + p[3] + 0x5A5A5A5AUL;
}
static int cal_load(void) {
    const cal_t *c = (const cal_t *)CAL_ADDR;
    if (c->magic != CAL_MAGIC || c->sum != cal_sum(c)) return 0;
    e_off = c->e_off;  pole_pairs = c->pole_pairs;
    return 1;
}
static int32_t cal_save(void) {
    if (pwm_run) return -1;                    /* never while driving */
    cal_t c = { CAL_MAGIC, 1, e_off, pole_pairs, 0, 0 };
    c.sum = cal_sum(&c);
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    FLASH_EraseInitTypeDef er = { .TypeErase = FLASH_TYPEERASE_PAGES,
                                  .Banks = FLASH_BANK_1, .Page = CAL_PAGE, .NbPages = 1 };
    uint32_t perr;
    if (HAL_FLASHEx_Erase(&er, &perr) != HAL_OK) { HAL_FLASH_Lock(); return -2; }
    const uint64_t *d = (const uint64_t *)&c;
    for (uint32_t i = 0; i < sizeof(c) / 8; i++)
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, CAL_ADDR + 8 * i, d[i]) != HAL_OK)
            { HAL_FLASH_Lock(); return -3; }
    HAL_FLASH_Lock();
    return cal_load() ? 0 : -4;                /* read back to verify */
}

static uint16_t enc_frame(void)
{
    uint16_t rx = 0;
    HAL_GPIO_WritePin(ENC_CS_GPIO_Port, ENC_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Receive(&hspi2, (uint8_t *)&rx, 1, 10);   /* receive only, never transmit */
    HAL_GPIO_WritePin(ENC_CS_GPIO_Port, ENC_CS_Pin, GPIO_PIN_SET);
    return rx;
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_ADC2_Init();
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  HAL_Delay(10);                                              /* settle */
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);

  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_ACCEPT_IN_RX_FIFO0,
                               FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_Start(&hfdcan1);

  txh.Identifier          = 0x123;
  txh.IdType              = FDCAN_STANDARD_ID;
  txh.TxFrameType         = FDCAN_DATA_FRAME;
  txh.DataLength          = FDCAN_DLC_BYTES_8;
  txh.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txh.BitRateSwitch       = FDCAN_BRS_OFF;
  txh.FDFormat            = FDCAN_CLASSIC_CAN;
  txh.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
  txh.MessageMarker       = 0;

  __HAL_DBGMCU_FREEZE_TIM1();                /* stop TIM1 when the debugger pauses */
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
  __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);   /* internal ADC trigger, no pin */
  HAL_ADCEx_InjectedStart_IT(&hadc1);



  //HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
  HAL_Delay(2);

  cal_ok = cal_load();                 /* restore e_off and pole_pairs */
  foc_mode = 0; ol_mode = 1; ol_v = 0.0f;
     /* first run: measure current offsets */

  foc_fault = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //calibration save
      if (save_req && !pwm_run) { save_res = cal_save(); save_req = 0; }

      if (rst) {
          rst = 0;
          for (int i = 0; i < 3; i++) { so_min[i] = 4095; so_max[i] = 0; }
          amin = 0xFFFF; amax = 0;
          vr_min = 4095; vr_max = 0;
      }
      /* drv8323h */
      HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin,
                        drv_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_GPIO_WritePin(DRV_CAL_GPIO_Port, DRV_CAL_Pin,
                        cal_on ? GPIO_PIN_SET : GPIO_PIN_RESET);
      HAL_ADC_Start(&hadc1);
      if (HAL_ADC_PollForConversion(&hadc1, 2) == HAL_OK) {
          vref_adc = HAL_ADC_GetValue(&hadc1);
          if (vref_adc < vr_min) vr_min = vref_adc;
          if (vref_adc > vr_max) vr_max = vref_adc;
          if (vref_adc > 0)
              vdda_mv = __HAL_ADC_CALC_VREFANALOG_VOLTAGE(vref_adc, ADC_RESOLUTION_12B);
      }
      /* PA6 nfault ADC2 */
      HAL_ADC_Start(&hadc2);
      adc_st = HAL_ADC_PollForConversion(&hadc2, 2);
      if (adc_st == HAL_OK) nf_adc = HAL_ADC_GetValue(&hadc2);

      /* AS5047P spi2 half duplex receive */
      /*
      raw = enc_frame();
      reads++;
      if (__builtin_parity(raw))      par_err++;
      else if (raw & 0x4000)          ef_cnt++;
      else {
          ang = raw & 0x3FFF;
          if (ang < amin) amin = ang;
          if (ang > amax) amax = ang;
      }
      */

      /* Encoder align */
      /*

      float mech = ang * (360.0f / 16384.0f);
      float raw_e = fmodf(mech * pole_pairs, 360.0f);
      if (align_req && pwm_run) { e_off = raw_e; align_req = 0; }
      e_deg = raw_e - e_off;
      if (e_deg < 0.0f) e_deg += 360.0f;
      */
      e_err = fmodf(e_deg - ol_deg + 540.0f, 360.0f) - 180.0f;   /* -180..+180 */

      /* FDCAN 100 ms/frame */
      static uint32_t tc = 0;
      if (HAL_GetTick() - tc >= 100) {
          tc = HAL_GetTick();
          txd[0] = (uint8_t)can_tx;            /* counter, so you can see frames change */
          if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txh, txd) == HAL_OK) can_tx++;
          else can_txerr++;
      }
      /* read */
      while (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) > 0) {
          if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxh, rxd) == HAL_OK) {
              can_rx++;
              can_rxid = rxh.Identifier;
          }
      }
      /* bus health */
      FDCAN_ErrorCountersTypeDef ec;
      FDCAN_ProtocolStatusTypeDef ps;
      HAL_FDCAN_GetErrorCounters(&hfdcan1, &ec);
      HAL_FDCAN_GetProtocolStatus(&hfdcan1, &ps);
      can_tec = ec.TxErrorCnt;  can_rec = ec.RxErrorCnt;  can_busoff = ps.BusOff;

      static uint8_t boot_cal = 0;
      if (!boot_cal && HAL_GetTick() > 1000) {     /* 1 s after reset */
          boot_cal = 1;
          foc_mode = 0; ol_mode = 1; ol_v = 0.0f;
          cal_req = 1; pwm_ms = 200; pwm_on = 1;
      }
      static uint8_t boot_ms = 0;
      if (boot_cal && !boot_ms && !pwm_run && !cal_req) {
          boot_ms = 1; pwm_ms = 2000;                /* restore normal run length */
      }

      /* PWM */

      if (pwm_ms > 10000) pwm_ms = 10000;   /*10s limit*/

      uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim1);

      if (!foc_mode) {
      if (ol_mode) {
          if (ol_v > 0.3f) ol_v = 0.3f;                    /* hard cap: ~3 A */
          float k = arr / 12.0f;
          if (pwm_run) ol_deg += ol_rate;
          if (ol_deg >= 360.0f) ol_deg -= 360.0f;
          if (ol_deg < 0.0f)    ol_deg += 360.0f;
          float th = ol_deg * 0.0174533f;
          float b  = arr / 2.0f;
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)(b + ol_v * k * cosf(th)));
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)(b + ol_v * k * cosf(th - 2.0944f)));
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (uint32_t)(b + ol_v * k * cosf(th + 2.0944f)));
      } else {
          for (int i = 0; i < 3; i++) if (duty[i] > 100) duty[i] = 100;
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, duty[0] * arr / 100);
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, duty[1] * arr / 100);
          __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, duty[2] * arr / 100);
      }
      }


      static uint32_t t_on = 0;
      if (pwm_on) { pwm_on = 0; if (!foc_fault) { pwm_run = 1; t_on = HAL_GetTick(); } }  /* request a run */
      if (pwm_run && (HAL_GetTick() - t_on >= pwm_ms)) pwm_run = 0;     /* time's up */

      if (drv_on && pwm_run) __HAL_TIM_MOE_ENABLE(&htim1);
      else                   __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);

      /* PB11 blink */
      static uint32_t t = 0;
      if (HAL_GetTick() - t >= 500) {
          t = HAL_GetTick();
          HAL_GPIO_TogglePin(TOGGLE_GPIO_Port, TOGGLE_Pin);
      }

      HAL_Delay(1);
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_VREFINT;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_1;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_24CYCLES_5;
  sConfigInjected.InjectedSingleDiff = ADC_SINGLE_ENDED;
  sConfigInjected.InjectedOffsetNumber = ADC_OFFSET_NONE;
  sConfigInjected.InjectedOffset = 0;
  sConfigInjected.InjectedNbrOfConversion = 3;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.QueueInjectedContext = DISABLE;
  sConfigInjected.ExternalTrigInjecConv = ADC_EXTERNALTRIGINJEC_T1_TRGO2;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONV_EDGE_FALLING;
  sConfigInjected.InjecOversamplingMode = DISABLE;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_2;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_2;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Injected Channel
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_3;
  sConfigInjected.InjectedRank = ADC_INJECTED_RANK_3;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc1, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.GainCompensation = 0;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_INTERNAL_LOOPBACK;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 2;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_1LINE;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
  htim1.Init.Period = 4250;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_OC4REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = 4249;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_ENABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 21;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, DRV_EN_Pin|DRV_CAL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TOGGLE_GPIO_Port, TOGGLE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ENC_CS_Pin|ENC_MOSI_HI_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DRV_EN_Pin DRV_CAL_Pin */
  GPIO_InitStruct.Pin = DRV_EN_Pin|DRV_CAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TOGGLE_Pin ENC_CS_Pin ENC_MOSI_HI_Pin */
  GPIO_InitStruct.Pin = TOGGLE_Pin|ENC_CS_Pin|ENC_MOSI_HI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *h)
{
    if (h->Instance != ADC1) return;
    uint32_t t0 = DWT->CYCCNT;

    /* --- currents --- */
    int32_t a = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_1);
    int32_t b = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_2);
    int32_t c = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_3);
    so[0] = a; so[1] = b; so[2] = c;

    static int32_t sa, sb, sc, n;
    if (cal_req && pwm_run && ol_mode && !foc_mode && ol_v == 0.0f) {
        sa += a; sb += b; sc += c;
        if (++n == 1024) {
            off[0] = sa / 1024; off[1] = sb / 1024; off[2] = sc / 1024;
            sa = sb = sc = n = 0; cal_req = 0;
        }
    }
    ia = (a - off[0]) * A_PER_COUNT;
    ib = (b - off[1]) * A_PER_COUNT;
    ic = (c - off[2]) * A_PER_COUNT;
    isum = ia + ib + ic;

    /* --- encoder (moved here from the main loop) --- */
    raw = enc_frame();
    reads++;
    if (__builtin_parity(raw))      par_err++;
    else if (raw & 0x4000)          ef_cnt++;
    else                            ang = raw & 0x3FFF;
    float mech  = ang * (360.0f / 16384.0f);
    float raw_e = fmodf(mech * pole_pairs, 360.0f);
    if (align_req && pwm_run) { e_off = raw_e; align_req = 0; }
    e_deg = raw_e - e_off;
    if (e_deg < 0.0f) e_deg += 360.0f;

    /* --- multi-turn position + PLL speed estimate --- */
    const float TS = 50e-6f, CNT2RAD = 6.2831853f / 16384.0f;
    static int32_t last_ang = -1, pos_cnt = 0;
    static float pos_est = 0.0f, vel_est = 0.0f;
    if (last_ang < 0) { last_ang = ang; }
    int32_t dcnt = (int32_t)ang - last_ang;
    if (dcnt >  8192) dcnt -= 16384;
    if (dcnt < -8192) dcnt += 16384;
    last_ang = ang;
    pos_cnt += dcnt;
    float pos_meas = pos_cnt * CNT2RAD;

    float perr = pos_meas - pos_est;
    pos_est += TS * (vel_est + 2.0f * pll_bw * perr);
    vel_est += TS * (pll_bw * pll_bw * perr);
    pos = pos_meas;  vel = vel_est;  rpm = vel_est * 9.5493f;

    /* --- overspeed trip --- */
    if (pwm_run && fabsf(vel_est) > vel_trip) {
        __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);
        pwm_run = 0; foc_fault = 2;
    }

    /* --- velocity loop -> iq command --- */
    static float vel_int = 0.0f;
    if (vel_mode && foc_mode && pwm_run) {
        float ve = vel_ref - vel_est;
        vel_int = fminf(fmaxf(vel_int + vel_ki * ve * TS, -i_max), i_max);
        iq_cmd  = fminf(fmaxf(vel_kp * ve + vel_int, -i_max), i_max);
    } else {
        vel_int = 0.0f;
        iq_cmd  = iq_ref;
    }

    /* --- hard overcurrent trip --- */
    if (pwm_run && drv_on && !cal_req &&
            (fabsf(ia) > i_trip || fabsf(ib) > i_trip || fabsf(ic) > i_trip)) {
        __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(&htim1);
        pwm_run = 0; foc_fault = 1;
    }

    /* --- current loop --- */
    static float id_int = 0.0f, iq_int = 0.0f;
    if (foc_mode && pwm_run) {
        float th = e_deg * 0.0174533f, s = sinf(th), co = cosf(th);

        float ial = ia;                              /* Clarke */
        float ibe = (ia + 2.0f * ib) * 0.57735f;
        id =  ial * co + ibe * s;                    /* Park */
        iq = -ial * s  + ibe * co;

        float idr = fminf(fmaxf(id_ref, -i_max), i_max);
        float iqr = fminf(fmaxf(iq_cmd, -i_max), i_max);
        float vlim = 0.9f * VBUS * 0.57735f;

        float ed = idr - id, eq = iqr - iq;          /* PI */
        id_int = fminf(fmaxf(id_int + ki_ts * ed, -vlim), vlim);
        iq_int = fminf(fmaxf(iq_int + ki_ts * eq, -vlim), vlim);
        float vdd = kp * ed + id_int, vqq = kp * eq + iq_int;
        float vm = sqrtf(vdd * vdd + vqq * vqq);
        if (vm > vlim) { vdd *= vlim / vm; vqq *= vlim / vm; }
        vd = vdd; vq = vqq;

        float val = vdd * co - vqq * s;              /* inverse Park */
        float vbe = vdd * s  + vqq * co;
        float Va = val;                              /* inverse Clarke */
        float Vb = -0.5f * val + 0.866025f * vbe;
        float Vc = -0.5f * val - 0.866025f * vbe;
        float vo = 0.5f * (fmaxf(Va, fmaxf(Vb, Vc)) + fminf(Va, fminf(Vb, Vc)));

        uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim1);
        float k = arr / VBUS, mid = arr * 0.5f;
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint32_t)(mid + (Va - vo) * k));
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, (uint32_t)(mid + (Vb - vo) * k));
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, (uint32_t)(mid + (Vc - vo) * k));
    } else {
        id_int = iq_int = 0.0f;                      /* reset when not running */
    }

    isr_cycles = DWT->CYCCNT - t0;
    isr_cnt++;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
