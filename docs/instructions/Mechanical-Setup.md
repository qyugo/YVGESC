---
title: Mechanical Setup
parent: Instructions
nav_order: 1
---

## Mechanical Setup

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/7d74cfa5-502e-4dc7-bd82-fde934b93e7b" alt="Mounted on custom motor, side view" style="width: 39%;">
  <img src="https://github.com/user-attachments/assets/b91d1f64-945a-4a23-8bc6-2674d9172a56" alt="Mounted on custom motor, oblique view" style="width: 51%;">
</div>

### 1. Mounting on Motor
The PCB contains M3-sized (3.00mm) mounting holes, with center spacing spacing X = 65mm Y = 55mm.

An adapter to mount the board on the underside of a motor can be fabricated, with adequate spacing for the end of the motor shaft and the encoder magnet carrier.

You can also check out the YVCase 2 which can act as a stand and a protector for the components.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/36575a5d-f015-4d60-ba58-37103177f367" style="width: 47%;">
  <img src="https://github.com/user-attachments/assets/ad7c76f5-7f97-453f-bd15-19096099ac93" style="width: 42%;">
</div>


### 2. Encoder and Magnet Setup

AS5047P encoder requires a 6x2.5mm, diametrically magnetized magnet.
 - https://www.digikey.com/en/products/detail/radial-magnet-inc/9042/469-1070-ND/5640338

Magnet must be mounted to the bottom of the motor's spinning shaft, with an airgap of approximately 2-3mm to the AS5047P chip on the bottom of the board. Epoxy or adhesive recommended, but a good press-fit has usually done the trick if toleranced carefully.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/0a718899-83dc-420f-b41d-a1fb3be944c1" style="width: 45%;">
  <img src="https://github.com/user-attachments/assets/299ba573-7934-4da6-a74b-6f7e60281195" style="width: 45%;">
</div>


### 3. Heatsinking

It is recommended to mount the PCB onto a metal chassis or heat spreader if operating at higher currents, as no onboard heat sinks are found. Additionally, a cooling fan is recommended for general operations exceeding 20A, but this may also depend on the motor used and its continuous current rating and/or phase current density exceeding ~6A/mm^2.
