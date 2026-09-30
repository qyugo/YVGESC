---
title: Firmware Setup
parent: Instructions
nav_order: 3
---

# Firmware Setup

Here is the initial process for flashing the board via ST-link. The example shown here will utilize a Nucleo-G431RB board, that runs the same G4RBT6 chip that the YVGESC uses.

### Part 1: ST-Link with Nucleo G431RB

Summary:
For first bring-up, a separate Nucleo-G431RB board containing an onboard ST-link debugger is used and programs the PCB via serial wire debug. Further, in case the board is ever bricked and the USB loses functionality, SWD will act as an emergency access point.

You can use a 1.27mm to 1.27mm 2x5 SWD cable, or connect them individually.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/ded3915e-7562-4e64-8fe1-10b6e5d3218d" alt="St-linked" style="width: 40%;">
  <img src="https://github.com/user-attachments/assets/e442a36a-32ce-4429-acb0-5009e2fba9c5" alt="YVGESC SWD Pinout" style="width: 40%;">
</div>

## 1A. Configuring Nucleo-G431RB

The Nucleo board has various jumpers with disconnectable 2-pin headers JPx and some CNx. As a default, JP1, JP7 should be open (no header), and JP3, JP5 (2->1) JP6,  JP8 should be bridged. Additional headers of CN11, CN12 should remain bridged to GND.

The only change needed is disconnecting JP3. What this does is allow the usage of nRST to only apply to the YVGESC, and not reset the Nucleo evaluation board itself.

<img src="https://github.com/user-attachments/assets/d6a9f1b9-d3e3-4ea2-aa80-5233347bf1f5" width=80%/>







SWCLK, SWDIO, GND, and 3.3 connected to Nucleo’s CN4. Please double check the wiring from the two SWD headers above.

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


