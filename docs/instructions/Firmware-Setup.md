---
title: Firmware Setup
parent: Instructions
nav_order: 3
---

# Firmware Setup

Here is the initial process for flashing the board via ST-link. The example shown here will utilize a Nucleo-G431RB board, that runs the same G4RBT6 chip that the YVGESC uses.

## Part 1: ST-Link with Nucleo G431RB

Summary:
For first bring-up, a separate Nucleo-G431RB board containing an onboard ST-link debugger is used and programs the PCB via serial wire debug. Further, in case the board is ever bricked and the USB loses functionality, SWD will act as an emergency access point.

You can use a 1.27mm to 1.27mm 2x5 SWD cable, or connect them individually.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/ded3915e-7562-4e64-8fe1-10b6e5d3218d" alt="St-linked" style="width: 40%;">
  <img src="https://github.com/user-attachments/assets/e442a36a-32ce-4429-acb0-5009e2fba9c5" alt="YVGESC SWD Pinout" style="width: 50%;">
</div>

### A. Configuring Nucleo-G431RB Firmware

The Nucleo board has various jumpers with disconnectable 2-pin headers JPx and some CNx. As a default, JP1, JP7 should be open (no header), and JP3, JP5 (2->1) JP6,  JP8 should be bridged. Additional headers of CN11, CN12 should remain bridged to GND. Ensure that these are set for this step.

Many softwares are needed from the ST multiverse. 

- STMCubeIDE
- STMCubeProgrammer

For configuring a Nucleo board, base firmware to turn it into a standalone ST-Link can be done in STMCubeIDE. Open a new project:

<img src="https://github.com/user-attachments/assets/99ea82e7-f898-4564-9887-1f76ebcfe322" width="90%" />

Flash the following in ```main.src``` to disconnect the debugger module in the Nucleo-G431RB:

```
c
#include <stdint.h>

#define REG(addr)    (*(volatile uint32_t *)(addr))
#define RCC_AHB2ENR  REG(0x4002104CUL)   // RCC base 0x40021000 + 0x4C
#define GPIOA_MODER  REG(0x48000000UL)
#define GPIOA_PUPDR  REG(0x4800000CUL)
#define GPIOB_MODER  REG(0x48000400UL)
#define GPIOB_PUPDR  REG(0x4800040CUL)

int main(void)
{
    RCC_AHB2ENR |= (1u << 0) | (1u << 1);
    (void)RCC_AHB2ENR;

    GPIOA_PUPDR &= ~(0xFu << 26);          // disable PA13 pullup, PA14 disable pulldown
    GPIOA_MODER |=  (0xFu << 26);          // PA13&PA14 to analog

    GPIOB_PUPDR &= ~(0x3u << 6);           // Disable pb3 pulldown
    GPIOB_MODER |=  (0x3u << 6);           // PB3 to analog

    for (;;) __asm volatile ("wfi");
}
```
If done successfully, there should be an error message as the board is eventually put to sleep:
<img width="635" height="178" alt="image" src="https://github.com/user-attachments/assets/f2dbbde3-54b2-4727-a014-295ec875802e" />

In STM32CubeProgrammer, it should not be visible unless the connection mode is set to "Under reset."

<img width="301" height="560" alt="image" src="https://github.com/user-attachments/assets/4ae70dc3-a4ea-46ad-84cd-4357de13a61f" />

Verify that the address is 0x468.

### B. Nucleo Hardware Interface

As mentioned prior, there are jumpers on the Nucleo board to act as hardware interfaces for certain settings.

After the ST-link isolation is flashed, the only change needed is disconnecting JP3. What this does is allow the usage of nRST to only apply to the YVGESC, and not reset the Nucleo evaluation board itself.

<img src="https://github.com/user-attachments/assets/d6a9f1b9-d3e3-4ea2-aa80-5233347bf1f5" width="80%"/>

### C. Connecting YVGESC to Nucleo/ST-Link

Both LEDs on the YVGESC should display, with 3.3V on VDD to GND, and ~10mA current draw at 12V powered on via the XT60.

There may be some inrush, but testing voltage incrementally should work, and keeping a bench PSU limit of 100mA. The LEDs shouldn't turn on, nor should there be any voltage on VDD until about 8-9V,  when the LM5164 begins to operate.


#### SWD Cable Vasectomy*
If you are using a 2x5 ribbon connector for SWD, ensure the VCC line (usually marked on the physical wire bundle) is disconnected, if not done already. The explanation is described in [Electrical Setup](Electrical-Setup.md).
If you are using individual cables, omit the connection of VCC, and only connect SWDIO, SWCLK, and GND.
SWCLK, SWDIO, and GND connected to Nucleo’s CN4. Please double check the wiring from the two SWD headers above.

*** IMPORTANT ***
Please follow the following order of operations when connecting ST-Link to the YVGESC:

1. Everything disconnected
2. SWD connecting both boards
3. XT60 powering YVGESC @ ~12V
4. Nucleo/ST-link to USB
5. Connect in STMCubeProgrammer. Port -> SWD; Mode -> Under reset; Reset Mode -> Hardware reset

To disconnect, perform these steps in reverse, disconnecting STMCubeProgrammer and the Nucleo's USB first, before powering down the YVGESC.

Once connected in CubeProgrammer, open Option Bytes from the left sidebar, and set ```nSWBOOT0 = 1```, and ```nBOOT0 = 1```. Hit "apply."

<img width="852" height="524" alt="image" src="https://github.com/user-attachments/assets/180e22a3-971f-49ba-89ab-d83be03e1d49" />

At this point, you can test out a [blink].

















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


