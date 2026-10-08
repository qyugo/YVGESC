---
title: 1.0 Firmware Documentation
parent: FOC Documentation
nav_order: 1
---

# Documentation for 0.0 Firmware to 1.0 Firmware

This documents the hardware, firmware, and software bring-up and testing process for developing all V0 versions up to V1.0, which is the first stable release.

## 1. Blink, Clock Config (V0.0)

1. Blink test on PB11 (33), toggle 2000ms

Configures PB11 (33) as blink GPIO for 0-3.3V oscillation of 2000ms, for verifying clock configuration. 

Initial testing firmware is configured in STM32CubeMX. STM32CubeMX was used to generate an STM32CubeIDE project for blink.

Here is a quick setup for flashing a 0-3.3v blink on the PB11 pin measured via oscilloscope.

From the Pinout and Configuration tab on the home page, switch to System View and configure the following:

SYS: Debug -> Serial Wire

<img width="503" height="310" alt="image" src="https://github.com/user-attachments/assets/39994454-9a9f-47b6-8d1e-ab522155912b" />

<img width="506" height="309" alt="image" src="https://github.com/user-attachments/assets/c5a6e27e-7244-4917-80a7-e17a1454a782" />

The oscillator that this board uses is an Abracon AB8MG. From the Clock tab, set input frequency to 8MHz, and HSE at System Clock Mux and PLL Source Mux.

<img width="1413" height="686" alt="image" src="https://github.com/user-attachments/assets/98adffba-1833-4f0f-a192-1a2663f76caf" />

Timer toggle:

<img width="517" height="559" alt="image" src="https://github.com/user-attachments/assets/ed1acf41-6bb0-4f73-9f0c-f7ca3b9a1fca" />

<img width="1267" height="770" alt="image" src="https://github.com/user-attachments/assets/64430369-78c6-42a8-ac9f-762ce62374bc" />


## 2. AS5047P Encoder

CubeMX was used to generate a CMake project for the rest of the v1p0 firmware.

The encoder is wired in at SPI2 (PB12, 13, 14, 25) for CSN, CLK, MOSI, and MISO

I wired PB14 to MOSI and PB15 to MISO mistakenly, so this will be changed if there is a V2 version of the board.

i. Instead of full duplex master, PB14->MOSI will need to be a permanently high GPIO and SPI2 will be configured in Half Duplex Master mode.

ii. Additionally, PB12 (ENC_CS) is set to HIGH GPIO. With NSS disabled, the pinout should show ENC_CS instead of SPI2_NSS.

iii. AS5047P encoders deliver 16 bits per frame and runs on a 2-Edge clock phase, so the setting must match accordingly.

<img width="993" height="649" alt="image" src="https://github.com/user-attachments/assets/97cb666a-ae78-44de-a773-1230b87d9f3a" />

<img width="433" height="190" alt="image" src="https://github.com/user-attachments/assets/fffce2fd-7c83-483f-8d78-fc3fd4c9487e" />


SPI2 is effectively only configured to receive. The functions ```HAL_SPI_Transmit``` and ```HAL_SPI_TransmitReceive``` therefore, should never be called.

## 3. Gate Driver

1. DRV_Enable

The MCU on the YVGESC maps PC0 (8) to DRV_ENABLE. In Pinout view, this is set to GPIO output, ensuring output level: LOW in System Core -> GPIO.

<img width="604" height="643" alt="image" src="https://github.com/user-attachments/assets/7295d300-2313-416a-9930-deacb20ec107" />

2. DRV_nFAULT is set to PA6 in hardware, so it is configured to GPIO input in CubeMX:

<img width="862" height="527" alt="image" src="https://github.com/user-attachments/assets/9d016068-a089-40f8-92ea-2b9b2b37b520" />



This is where I encountered the first issue, as ADC was not properly reading the nFAULT signals. This is later fixed via hardware and in the 

Correction (unused): Led and fault, 100k resistor for v2

Notes: PA6 to ADC2_IN3 Watchdog
VREFINT Debug

<img width="634" height="749" alt="image" src="https://github.com/user-attachments/assets/32cc4964-66f7-4c5d-8a65-af22f68cd481" />

SOC and Solder

VREF Soldered to 3.3V (issue 3, bringup for yvgesc v2)
SOC multimeter/scope checks

## 4. Current Sense

<img width="829" height="604" alt="image" src="https://github.com/user-attachments/assets/86b98d27-559c-43a2-bcc3-538bcecf51bf" />


<img width="875" height="381" alt="image" src="https://github.com/user-attachments/assets/4b7fa4a9-0000-4946-b917-02c627a5c96a" />

<img width="904" height="388" alt="image" src="https://github.com/user-attachments/assets/b9b1bf99-75d0-453c-909a-61839049b5d0" />

I had Vrefint wrongly configured. Update with CAL_ON = 1:

<img width="897" height="384" alt="image" src="https://github.com/user-attachments/assets/a600886a-8441-4d7f-8407-2c4b80fc8fb2" />

...and CAL_ON=0:

<img width="907" height="428" alt="image" src="https://github.com/user-attachments/assets/6d248916-da42-42aa-82bc-d24fe62cbb68" />

There is some noise, may try to reduce using oversampling later on, there is likely some noise nearby due to all the soldering or otherwise.

Current sense complete, next up will be the FDCAN and first PWM drive.

ADC and everything works. Released as 0.1.

## 5. FDCAN

PB8 is wired to CAN_RX, and PB9 is wired to CAN_TX, clocked to HSE @ 8MHz.

<img width="259" height="141" alt="image" src="https://github.com/user-attachments/assets/f2dcf289-a694-443f-bff9-dc8150097926" />

Test 1: internal loopback, 500kbits/s, sample point 87.5%

<img width="892" height="333" alt="image" src="https://github.com/user-attachments/assets/7c432289-d4d7-40a9-bb99-ab509535d480" />

Will test Normal mode with another can bus to test for receiving messages as well as the termination switch.

## 6. PWM + Open Loop

Pinout:
INHA: PA8
INLA: PA7
INHB: PA9
INLB: PB0
INHC: PA10
INLC: PB1

<img width="409" height="385" alt="image" src="https://github.com/user-attachments/assets/97683e3f-fc28-42ba-8752-6f1a8aaf6abe" />

Center aligned, 20kHz, 500ns dead time (currently x4)

Works.

CLock to PLL, 8-170MHz

pllm to /2

<img width="1381" height="550" alt="image" src="https://github.com/user-attachments/assets/4ba9676f-efb4-4a84-9ee4-4a67534a1711" />

124ns tim1 dead time

SPi2 prescaler to 5.3125 mHZ

Open loop success. Released as 0.2.

## 7. Phase Current Sampling for FOC Loops

In this part, the sampling is configured to happen at a fixed point in each PWM cycle. The shunts are located on the low side FETs.

A PWM generation (No Output) channel is set for CH4, acting basically as a comparator:
<img width="492" height="97" alt="image" src="https://github.com/user-attachments/assets/4f9cc67b-4a98-4334-bd5f-0bbcf8469d85" />

VREF, initially part of the 4 injected channels, is now in the regular conversion channel. 


<img width="478" height="220" alt="image" src="https://github.com/user-attachments/assets/81e3cf7b-f5e1-45a4-bc88-2e0e0aa11a37" />

For the injected conversions, the sample is now performed on the falling edge of Timer 1 Trigger Out Event 2.

The interrupt line in USER CODE 4 is implemented as follows:

```
c
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *h)
{
    if (h->Instance != ADC1) return;
    int32_t a = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_1);
    int32_t b = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_2);
    int32_t c = HAL_ADCEx_InjectedGetValue(h, ADC_INJECTED_RANK_3);
    so[0] = a; so[1] = b; so[2] = c;

    static int32_t sa, sb, sc, n;                 /* offset calibration */
    if (cal_req && !pwm_run) {
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
    isr_cnt++;
}
```

Calibrating (cal_req=1, pwm_on=1, ol_mode=1, ol_v = 0.12

<img width="891" height="651" alt="image" src="https://github.com/user-attachments/assets/672f4748-b830-4513-8fee-e280cea1bf8f" />

Current sums roughly 0:

<img width="818" height="134" alt="image" src="https://github.com/user-attachments/assets/3d361b03-3ec7-4217-87f0-8bf696e3543a" />

Calibration with encoder adn offset:

<img width="752" height="260" alt="image" src="https://github.com/user-attachments/assets/1f493330-2656-4d0f-bac9-f04855f659d9" />

<img width="746" height="250" alt="image" src="https://github.com/user-attachments/assets/a4f801f8-be34-431e-9215-6ef0e6b4b3a6" />


<img width="745" height="261" alt="image" src="https://github.com/user-attachments/assets/67db0fd1-1e91-4000-9bcb-e95844fd250c" />

<img width="738" height="250" alt="image" src="https://github.com/user-attachments/assets/ee74c029-84ee-4b17-afc5-a8d5bcdf5914" />

<img width="749" height="222" alt="image" src="https://github.com/user-attachments/assets/ff96b317-36ab-4df4-8e38-20b91074bfee" />

ia, ib, ic are amps of the measured phase currents by the ADC via its $1 m\Omega$ shunt and multiplied by the gain of 40V/V.

Contextualization
Next, ia, ib, and ic are used in the Clarke transforms to become $I\alpha$ and $I\beta$, into a 2-part current descriptor in reference to the stator frame.

The park transform then converts $I\alpha$ and $I\beta$ into Iq and Id, in reference to the rotor axis.

The current loop then keeps Id at 0 and Iq relative to a torque command.

## 8. CURRENT LOOP - Running inside the ADC2 interrupt (20kHz)

What it does: measures current and transforms them into Id and Iq using encoder angle, with two PI controllers

New variables:
```
c
volatile uint32_t foc_mode = 0;          /* 1 = current loop drives the PWM */
volatile float id_ref = 0.0f, iq_ref = 0.0f;   /* commanded currents, A */
volatile float id, iq, vd, vq;           /* measured currents, output voltages */
volatile float kp = 0.3f;                /* V/A   (L × ωc, ωc = 2π·1 kHz) */
volatile float ki_ts = 0.03f;            /* V/A per sample (R × ωc × Ts) */
volatile float i_max = 2.0f;             /* command limit, A */
volatile float i_trip = 8.0f;            /* instant shutdown, A */
volatile uint32_t foc_fault = 0, isr_cycles = 0;
#define VBUS 12.0f
```

Interrupt callback:
```
c
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

    /* --- hard overcurrent trip --- */
    if (fabsf(ia) > i_trip || fabsf(ib) > i_trip || fabsf(ic) > i_trip) {
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
        float iqr = fminf(fmaxf(iq_ref, -i_max), i_max);
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
```

Testing order:
foc_mode = 0, ol_mode = 1, ol_v = 0, cal_req = 1, pwm_on = 1

ol_v = 0.15, ol_deg = 0, -> run -> align_req = 1

## 9. TORQUE MODE/closed loop current, and calibration script 

Minimum Iq = 2 to spin for Eaglepower

Torque mode / inner PI loops complete

FLASH update to store calibrations:
```c FLASH    (rx)    : ORIGIN = 0x8000000,   LENGTH = 126K ```

AFter good alignment: save_req = 1, save_res = 0 in debug

kp = 0.3, ki = 0.03

Works with flash calibration. If needed, realign and hit save_req=1, otherwise calibration should happen automatically upon boot.

## 10. VELOCITY CONTROL

New variables:
```
c
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
```
Interrupt:
```
c
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
```
New fault label: foc_fault 2 = overspeed, foc_faul 1 = overcurrent

Kp 0.2, Ki 15

Some cogging stuff at lower speeds, bumped up ki, bumping up kp gives a grinding noise not from the motor.

Released as V0.4 in firmware.

## 11. Position and Impedance Control

Objective: spring damper with feed forward torque t_ff.

Success.

TODO: User-friendly settings for stiff, medium, and loose compliance, as well as manual tuning via CAN.

## 12. CAN from another source/telemetry tests + YVGESC Control Software

Using first a CANable isolator to send telemtry directly from a PC, a python script was developed, which later was developed into the YVGESC UI. All debugging parameters, including zeroing, calibration, open loop controls, FOC controls, and tuning parameters were sent to the CAN bus in the callback section of the firmware. More details can be found in [CAN Setup](setups/CANSetup.md). 

Given at this point the firmware already runs a calibration script, the CAN python UI offers preprogrammed buttons that run times sequences for calibration, zeroing, and encoder alignment, in case things get messy during testing. At this point, the alignment occasionally falls, and will be debugged in later versions.

Further, as the software clears faults automatically if reset, an additional reset diagnosis from the previous fault is implemented.

```
c

//Private variable
volatile uint32_t rst_csr = 0;
// PV end

  /* USER CODE BEGIN 1 */

	rst_csr = RCC->CSR;
	   RCC->CSR |= RCC_CSR_RMVF;

  /* USER CODE END 1 */
/*USER CODE BEGIN 4 */
	/* 53 */ { &rst_csr, VT_U, 0 },
*/
```
In the software, fault causes are described in plain language in the UI.
```
python
def reset_cause(csr):
    flags = [(31, "low-power"), (30, "WWDG"), (29, "IWDG"), (28, "software"),
             (27, "brown-out"), (26, "reset pin"), (25, "option bytes")]
    hits = [n for b, n in flags if csr & (1 << b)]
    return ", ".join(hits) if hits else hex(csr)
```

At this point, most of my testing runs had a hard 10s limit. I'd still like to keep this limit on in the meantime for personal use, but I increased it to 60s for longer tests. An additional script to stop the board in 1s if the CAN cables happen to lose connection. 
Note* If in debugger mode, keepalive needs to be set to 0 since it's not in CAN mode.

```
c
volatile uint32_t last_param_ms = 0, keepalive = 1;

if (pwm_ms > 60000) pwm_ms = 60000;   /*60s limit*/

static void param_req(const uint8_t *d)
{
	last_param_ms = HAL_GetTick();
    FDCAN_TxHeaderTypeDef th = txh;
    th.Identifier = 0x580 + NODE_ID;
    th.DataLength = FDCAN_DLC_BYTES_8;

    uint8_t r[8] = { 0 };
    uint8_t op = d[0], idx = d[1];
    r[1] = idx;

/* CAN*/
      static uint32_t t_on = 0;
      static uint8_t was_can = 0;
      uint32_t lc = last_cmd_ms;
      if (can_en && (HAL_GetTick() - lc > 100)) {                 /* 100 ms timeout */
                can_en = 0; can_timeout_cnt++;
            }
      if (can_en) {
          pwm_run = foc_fault ? 0 : 1;                           /* CAN in control */
      } else {
          if (was_can) pwm_run = 0;                              /* CAN just stopped */
          if (pwm_on) { pwm_on = 0; if (!foc_fault) { pwm_run = 1; t_on = HAL_GetTick(); } }
          if (pwm_run && (HAL_GetTick() - t_on >= pwm_ms)) pwm_run = 0;
          uint32_t lp = last_param_ms;
          if (keepalive && pwm_run && (HAL_GetTick() - lp > 1000))
             pwm_run = 0;
      }
      was_can = can_en;
```
*AT THIS POINT, THE BOARD SHOULD STILL DRAW ~0.026A WHEN IDLE @ 12V.

*TODO:*

## 13. DFU, BOOT0, and USB

## 14. USB SLCAN

## 15. Flux Braking (Advanced)






