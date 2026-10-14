---
title: CAN Setup + GUI
nav_order: 6
has_children: true
---

# CAN Setup + Python UI

In the V0 firmware, only internal loopback was configured for CAN. 
V1 firmware requires a CAN adaptor as USB->SLCAN had not been configured, but should work without the setup steps.

Check the instructions for the UI usage and variable nomenclature [here](YVGUIinstructions.md), as V1 directly references the debug variables.

[Download V1 firmware](../1p0)
<br>
[Download CAN UI](YVCAN1p0.py)

<iframe
  src="https://www.youtube.com/embed/SErpxAt9m6E"
  style="width:100%; aspect-ratio:16/9; border:0;"
  title="CAN setup"
  loading="lazy"
  allowfullscreen>
</iframe>

<img width="722" height="774" alt="image" src="https://github.com/user-attachments/assets/67f6f867-37d8-43b1-8588-f8ca0d426409" />


Here, CAN was configured to send parameters and telemetry from both a computer using a CANAble and through another board. If only using two nodes (one sender, one receiver), the $120\Omega$ switch should be turned ON on both sides. Therefore, the resistance from CANH to CANL should be $60\Omega$.

## Setup Documentation

1. USB to CAN Adapter

<img width="707" height="376" alt="Screenshot 2026-10-07 at 7 29 55 PM" src="https://github.com/user-attachments/assets/e6a16f88-8b01-46de-a981-bdcb7a190996" />

CANH Pin 1, CANL Pin 2

<img width="513" height="291" alt="Screenshot 2026-10-07 at 7 30 58 PM" src="https://github.com/user-attachments/assets/ae411e69-0a24-4399-bdd4-ea7f03e1b9c6" />

For my connectors, yellow is CANH, black is CANL, red is GND.

<img width="859" height="177" alt="image" src="https://github.com/user-attachments/assets/5039d268-01d8-4d70-8719-4f9189fe44a0" />
<img width="595" height="253" alt="image" src="https://github.com/user-attachments/assets/da9ef1ac-d9ac-497f-a8b7-1d9ed188eb04" />

In NVIC, interrupt priority is set to ADC before FDCAN1:

<img width="619" height="188" alt="image" src="https://github.com/user-attachments/assets/d0193883-856a-41ed-be55-33c804fd010a" />






