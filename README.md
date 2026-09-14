# YVGESC
A custom FOC Electronic Speed Controller for running brushless motors.
Up to 50V bus, 30A cont. phase current

Running a STM32G431RBT6 IC and DRV8323H gate driver, with an AS5047P absolute encoder.

Features:
- USB + SWD for flashing and debugging
- Firmware controlled regen heat dissipation to motor windings (via VBUS sense)
- 2x CAN line for board daisy-chaining
- Supports USB to SLCAN
- Sparkly LEDs (1 red, 1 green)


Hardware and firmware guides in docs.

<img width="1194" height="695" alt="Screenshot 2026-08-26 at 7 20 42 PM" src="https://github.com/user-attachments/assets/f3298a6a-bc50-4d27-b1bd-ad60d90a4878" />



