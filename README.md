---
title: Overview
nav_order: 1
permalink: /
---



# YVGESC
A custom FOC Electronic Speed Controller for running brushless motors.
Up to 50V bus, 30A cont. phase current

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/5b07d36f-3da9-4ea6-a4c7-dd8945dfa303" alt="Mounted on custom motor" style="width: 35%;">
  <img src="https://github.com/user-attachments/assets/d7143a77-d2e6-44ee-945c-3f06ae84d620" alt="Driver boards, front and back view" style="width: 40%;">
</div>


Running a STM32G431RBT6 IC and DRV8323H gate driver, with an AS5047P absolute encoder.

Features:
- USB + SWD for flashing and debugging (requires ST-link)
- Firmware controlled regen heat dissipation to motor windings
- 2x CAN line for board daisy-chaining
- Supports USB to SLCAN
- Sparkly LEDs (1 red, 1 green)


Hardware and firmware guides in docs.

<img width="1194" height="695" alt="Screenshot 2026-08-26 at 7 20 42 PM" src="https://github.com/user-attachments/assets/f3298a6a-bc50-4d27-b1bd-ad60d90a4878" />



