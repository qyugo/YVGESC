---
title: Firmware Setup
parent: Instructions
nav_order: 3
---

# Firmware Setup

Here is the initial process for flashing the board via ST-link. The example shown here will utilize a Nucleo-G431RB board, that runs the same G4RBT6 chip that the YVGESC uses.

## STM32 Hardware Setup

### 1. Initial Setup: SWD

<img src="https://github.com/user-attachments/assets/29a40d64-4829-4905-839f-94ab0b0469a0" width="50%" />

For first bring-up, a separate Nucleo-G431RB board containing an onboard ST-link debugger is used and programs the PCB via serial wire debug.

Further, in case the board is ever bricked and the USB loses functionality, SWD will act as an emergency access point.

SWCLK, SWDIO, GND, and 3.3 connected to Nucleo’s CN4. Please double check the wiring from the two SWD headers:

Firmware: nBOOT_sel = 1

2. USB Setup (CDC, DFU)

After initial bring-up, USB can be configured to act as a virtual COM port for future device flashing or firmware updates.

3. CAN Setup (USB to SLCAN)

Since the board is USB-C capable and contains an onboard CAN transceiver, it is possible to access CAN communications directly over USB without the need for a separate USB to CAN adapter (such as the CANable). Via FDCAN1 and the USB peripheral, commands can be translated via serial line (ASCII) between USB-CDC and the CAN bus. This will be implemented in firmware.


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


