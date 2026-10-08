# CAN Setup

Internal loopback was configured in V0 firmware.

Here, CAN will be configured to send parameters and telemetry from both a computer and through another board.

1. USB to CAN Adapter

<img width="707" height="376" alt="Screenshot 2026-10-07 at 7 29 55 PM" src="https://github.com/user-attachments/assets/e6a16f88-8b01-46de-a981-bdcb7a190996" />

CANH Pin 1, CANL Pin 2

<img width="513" height="291" alt="Screenshot 2026-10-07 at 7 30 58 PM" src="https://github.com/user-attachments/assets/ae411e69-0a24-4399-bdd4-ea7f03e1b9c6" />

For my connectors, yellow is CANH, black is CANL, red is GND.

<img width="859" height="177" alt="image" src="https://github.com/user-attachments/assets/5039d268-01d8-4d70-8719-4f9189fe44a0" />
<img width="595" height="253" alt="image" src="https://github.com/user-attachments/assets/da9ef1ac-d9ac-497f-a8b7-1d9ed188eb04" />

In NVIC, interrupt priority is set to ADC before FDCAN1:

<img width="619" height="188" alt="image" src="https://github.com/user-attachments/assets/d0193883-856a-41ed-be55-33c804fd010a" />

