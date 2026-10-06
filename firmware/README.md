---
title: FOC Documentation
nav_order: 5
has_children: true
---

# FOC Documentation

This contains the documentation for the first bring-up to basic FOC control (position, velocity, torque, impedance control in closed loop) via hardware corrections, firmware and software on the YVGESC-1.

If you received a board from me, you can head straight to the [Instructions](../docs/instructions/Firmware-Setup.md) section for first flash.

Current status: Torque, velocity, and position/spring damper control loops written, achieved and tuned.

Pre-releases:

| Version | Release Date | Download | Description |
|:--------|:-------------|:---------|:------------|
| v0.4.0 | 2026-10-3 | [firmware-v0p4](https://github.com/qyugo/YVGESC/tree/main/firmware/0p4-Velocity-PI-Loop) | Velocity PI loops. |
| v0.3.0 | 2026-10-2 | [firmware-v0p3](https://github.com/qyugo/YVGESC/tree/main/firmware/0p3-CURRENT-PI-LOOPS%2BCalibrationscript) | Current loops and calibration script. |
| v0.2.0 | 2026-9-30 | [firmware-v0p2](https://github.com/qyugo/YVGESC/tree/main/firmware/0p2-FDCAN-PWM/yvgesc-v1p0) | FDCAN and PWM development. |
| v0.1.0 | 2026-9-30 | [firmware-v0p1](https://github.com/qyugo/YVGESC/tree/main/firmware/0p1-AS5047P-DRV-NFAULT-ADC) | Encoder, ADC to DRV_nFault config after hardware fixes. |
| v0.0.0 | 2026-9-28 | [firmware-v0p0](https://github.com/qyugo/YVGESC/tree/main/firmware/0p0-blink-pgood-clock-swd) | Power regulation, Clock, and blink. |

Working on CAN implementation and possible a UI/some basic scripts to control the board parameters and send commands without CAN.

Down the line, USB and regen implementation will be performed.
