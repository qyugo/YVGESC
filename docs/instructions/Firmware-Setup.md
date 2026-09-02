**FIRMWARE SETUP for STM32G**

Softwares used:

1. **STM32CubeMX - Configure GPIOs, alternate pinout settings:**

  a) nboot_sel must be set to 1
    - BOOT will be locked at PB8 at the default nboot_sel=0. 
    - Select PB10 as the new BOOT pin.

  b) AF9 (alternate function 9) must be selected for FDCAN1 alternate pinout
    - PA11/12 is the default CAN_RX/CAN_TX setting.
    - CAN_RX and CAN_TX will be set to PB8 and PB9, respectively.

  c) USB to PA11/12

2. **STM32CubeMX - Set parameters and Generate C code**
    a) ST Motor Pilot used within MX to configure motor hardware.

3. **STM32CubeIDE - Compile and flash C code for MCU**


Flux Braking Setup - TBD
