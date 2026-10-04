FIRMWARE

1. CubeMX was used to generate an STM32CubeIDE project for blink.

Configures PB11 as blink GPIO for 0-3.3V oscillation of 2000ms, for checking clock.

3. CubeMX was used to generate a CMake project for v1p0 firmware.

### AS5047P Encoder

The encoder is wired in at SPI2 (PB12, 13, 14, 25) for CSN, CLK, MOSI, and MISO

I wired PB14 to MOSI and PB15 to MISO mistakenly, so this will be changed if there is a V2 version of the board.

i. Instead of full duplex master, PB14->MOSI will need to be a permanently high GPIO and SPI2 will be configured in Half Duplex Master mode.

ii. Additionally, PB12 (ENC_CS) is set to HIGH GPIO. With NSS disabled, the pinout should show ENC_CS instead of SPI2_NSS.

iii. AS5047P encoders deliver 16 bits per frame and runs on a 2-Edge clock phase, so the setting must match accordingly.

<img width="993" height="649" alt="image" src="https://github.com/user-attachments/assets/97cb666a-ae78-44de-a773-1230b87d9f3a" />

<img width="433" height="190" alt="image" src="https://github.com/user-attachments/assets/fffce2fd-7c83-483f-8d78-fc3fd4c9487e" />


SPI2 is effectively only configured to receive. The functions ```HAL_SPI_Transmit``` and ```HAL_SPI_TransmitReceive``` therefore, should never be called.

### Gate Driver

DRV_nFAULT is set to PA6 in hardware, so it is configured to GPIO input in CubeMX:

<img width="862" height="527" alt="image" src="https://github.com/user-attachments/assets/9d016068-a089-40f8-92ea-2b9b2b37b520" />

Correction: Led and fault, 100k resistor for v2

PA6 to ADC2_IN3 Watchdog
VREFINT Debug


<img width="634" height="749" alt="image" src="https://github.com/user-attachments/assets/32cc4964-66f7-4c5d-8a65-af22f68cd481" />

SOC and Solder

VREF Soldered to 3.3V (issue 3, bringup for yvgesc v2)
SOC multimeter/scope checks

Current Sense

<img width="829" height="604" alt="image" src="https://github.com/user-attachments/assets/86b98d27-559c-43a2-bcc3-538bcecf51bf" />


<img width="875" height="381" alt="image" src="https://github.com/user-attachments/assets/4b7fa4a9-0000-4946-b917-02c627a5c96a" />

<img width="904" height="388" alt="image" src="https://github.com/user-attachments/assets/b9b1bf99-75d0-453c-909a-61839049b5d0" />

I had Vrefint wrongly configured. Update with CAL_ON = 1:

<img width="897" height="384" alt="image" src="https://github.com/user-attachments/assets/a600886a-8441-4d7f-8407-2c4b80fc8fb2" />

...and CAL_ON=0:

<img width="907" height="428" alt="image" src="https://github.com/user-attachments/assets/6d248916-da42-42aa-82bc-d24fe62cbb68" />

There is some noise, may try to reduce using oversampling later on, there is likely some noise nearby due to all the soldering or otherwise.

Current sense complete, next up will be the FDCAN and first PWM drive.

## FDCAN

PB8 is wired to CAN_RX, and PB9 is wired to CAN_TX, clocked to HSE @ 8MHz.

<img width="259" height="141" alt="image" src="https://github.com/user-attachments/assets/f2dcf289-a694-443f-bff9-dc8150097926" />

Test 1: internal loopback, 500kbits/s, sample point 87.5%

<img width="892" height="333" alt="image" src="https://github.com/user-attachments/assets/7c432289-d4d7-40a9-bb99-ab509535d480" />

Will test Normal mode with another can bus to test for receiving messages as well as the termination switch.

## PWM

Pinout:
INHA: PA8
INLA: PA7
INHB: PA9
INLB: PB0
INHC: PA10
INLC: PB1

<img width="409" height="385" alt="image" src="https://github.com/user-attachments/assets/97683e3f-fc28-42ba-8752-6f1a8aaf6abe" />

Center aligned, 20kHz, 500ns dead time (currently x4)

WOrks

CLock to PLL, 8-170MHz

pllm to /2

<img width="1381" height="550" alt="image" src="https://github.com/user-attachments/assets/4ba9676f-efb4-4a84-9ee4-4a67534a1711" />

124ns tim1 dead time

SPi2 prescaler to 5.3125 mHZ




