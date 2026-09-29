---
title: Electrical Setup
parent: Instructions
nav_order: 2
---

## Electrical Setup

--- Considerations ---
* No onboard OCP. It is recommended to use an offboard Slo-Blo fuse rated for 58V, 30-50A depending on usage parameters.
* No onboard USB isolator. A separate isolator may be used if powered from PSU or battery while maintaining USB connection to prevent ground loops.
* No onboard reverse polarity protection, check XT60 connector orientation carefully before connecting.
* Board is rated for 6-50V, 2S-12S LiPo
* Inrush may occur if not using switch during LiPo battery connection.
* Be very careful with regeneration if using a PSU, an external regen clamp or implementation of flux braking in firmware is recommended before regen applications, as no brake resistor is implemented.

<img src="https://github.com/user-attachments/assets/b1d96820-d4d0-43a5-b19d-ffbca785534b" width="50%" />


--- Indicators ---

Green LED on: 3.3V regulation is working (PGOOD)

Red LED on: Gate driver fault detected (DRV_NFAULT)

## STM32 Hardware Setup

### 1. Initial Setup: SWD

<img src="https://github.com/user-attachments/assets/29a40d64-4829-4905-839f-94ab0b0469a0" width="50%" />

For first bring-up, a separate Nucleo-G431RB board containing an onboard ST-link debugger is used and programs the PCB via serial wire debug.

Further, in case the board is ever bricked and the USB loses functionality, SWD will act as an emergency access point.

SWCLK, SWDIO, GND, and 3.3 connected to Nucleo’s CN4.

Firmware: nBOOT_sel = 1

2. USB Setup (CDC, DFU)

After initial bring-up, USB can be configured to act as a virtual COM port for future device flashing or firmware updates.

3. CAN Setup (USB to SLCAN)

Since the board is USB-C capable and contains an onboard CAN transceiver, it is possible to access CAN communications directly over USB without the need for a separate USB to CAN adapter (such as the CANable). Via FDCAN1 and the USB peripheral, commands can be translated via serial line (ASCII) between USB-CDC and the CAN bus. This will be implemented in firmware.



