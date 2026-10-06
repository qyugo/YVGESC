---
title: Overview
nav_order: 1
permalink: /
---



# YVGESC
This is my custom driver board that runs Field-Oriented Control (FOC) for high-performance servo, speed, and torque control for brushless DC motors. 

It is a 64x74mm board I designed in KiCad and commissioned to JLCPCB, handling up to 50V bus, 30A continuous current per phase (if air-blasted), and a 52A peak current.

This is the final implementation in a full-stack actuator learning adventure I embarked on for fun, having built brushless motors from scratch, a motor-assembling visualizer, and a BLDC drivetrain. If you are interested, those projects are on my website.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/5b07d36f-3da9-4ea6-a4c7-dd8945dfa303" alt="Mounted on custom motor" style="width: 45%;">
  <img src="https://github.com/user-attachments/assets/d7143a77-d2e6-44ee-945c-3f06ae84d620" alt="Driver boards, front and back view" style="width: 45%;">
</div>

Running a STM32G431RBT6, a DRV8323H gate driver, and an AS5047P absolute encoder.

Hardware and firmware guides in docs.

## Features

The board features both SWD and USB for flashing and debugging, though the SWD requires an ST-Link that will be covered in this repo. 

There are two 1x3 CAN JST connectors on board, for communications and chaining multiple boards, requiring only one central CPU to control multiple actuators. Once set up as CDC, the USB can also act as a communications bridge to the CAN bus via SLCAN, eliminating the need for a CAN adapter (such as the CANable).

A red and green LED are placed onboard, indicating gate driver fault and feedback voltage exceeding limits on the buck regulator, respectively.

Instead of a brake resistor circuit, regeneration is controlled via firmware. Overvoltage due to regen is dissipated to motor windings as heat, similarly to the Moteus X1 board.

## Considerations

Additional hardware, such as CAN cables, an ST-Link board, 1.27mm SWD connectors, MIDI/ANL fuse, brake resistor, USB isolator, radial magnet, and XT60 input connector is outlined in the [Instructions](docs/instructions/README.md) tab.

The board does not natively support simultaneous usage of USB and power, unless an off-board USB galvanic isolator is used, to prevent ground loops. 

The board also does not contain onboard overcurrent protection, besides the OCP trip programmed in the gate driver. An inline MIDI fuse rated for 58V/50A can be used offboard as an extra safety measure.

Send an email to ukahan251@gmail.com for any questions or requests for more detailed documentations, which I will momentarily gatekeep.

<img width="1194" height="695" alt="Screenshot 2026-08-26 at 7 20 42 PM" src="https://github.com/user-attachments/assets/f3298a6a-bc50-4d27-b1bd-ad60d90a4878" />



