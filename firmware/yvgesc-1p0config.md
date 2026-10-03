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


<img width="634" height="749" alt="image" src="https://github.com/user-attachments/assets/32cc4964-66f7-4c5d-8a65-af22f68cd481" />

