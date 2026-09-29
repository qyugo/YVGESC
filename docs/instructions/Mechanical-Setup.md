---
title: Mechanical Setup
parent: Instructions
nav_order: 1
---

## Mechanical Setup

<img src="https://github.com/user-attachments/assets/b91d1f64-945a-4a23-8bc6-2674d9172a56" width = "45%" />
<img src="https://github.com/user-attachments/assets/299ba573-7934-4da6-a74b-6f7e60281195" width = "45%" />

### 1. Mounting
The PCB contains M3-sized mounting holes, with spacing X = 65mm Y = 55mm.

An adapter to mount the board on the underside of a motor can be fabricated, with adequate spacing for the end of the motor shaft and the encoder magnet carrier.

<img src="https://github.com/user-attachments/assets/36575a5d-f015-4d60-ba58-37103177f367" width="50%"/>

### 2. Encoder and Magnet Setup

AS5047P encoder requires a 6x2.5mm, diametrically magnetized magnet.
 - https://www.digikey.com/en/products/detail/radial-magnet-inc/9042/469-1070-ND/5640338

Magnet must be mounted to the bottom of the motor's spinning shaft, with an airgap of approximately 2-3mm to the AS5047P chip on the bottom of the board. Epoxy or adhesive recommended, but a good press-fit has usually done the trick if toleranced carefully.

<img src="https://github.com/user-attachments/assets/0a718899-83dc-420f-b41d-a1fb3be944c1" width="50%"/>

### 3. Heatsinking

It is recommended to mount the PCB onto a metal chassis or heat spreader if operating at higher currents, as no onboard heat sinks are found. Additionally, a cooling fan is recommended for general operations exceeding 20A, but this may also depend on the motor used and its continuous current rating and/or phase current density exceeding ~6A/mm^2.
