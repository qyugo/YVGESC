---
title: Electrical Setup
parent: Instructions
nav_order: 2
---

## Electrical Setup
Here is the electrical setup, including hardware requirements for power, CAN bus, safety measures, and firmware links.

---

# Considerations: 

1.  Board is rated for 6-50V, 2S-12S LiPo
2.  No onboard OCP. It is recommended to use an offboard Slo-Blo fuse rated for 58V, 30-50A depending on usage parameters.
3.  No onboard USB isolator. A separate isolator may be used if powered from PSU or battery while maintaining USB connection to prevent ground loops.
4.  No onboard reverse polarity protection, check XT60 connector orientation carefully before connecting.
5.  Inrush may occur if not using switch during LiPo battery connection.
6.  Be very careful with regeneration if using a PSU, an external regen clamp or implementation of flux braking in firmware is recommended before regen applications, as no brake resistor is implemented.

<img src="https://github.com/user-attachments/assets/b1d96820-d4d0-43a5-b19d-ffbca785534b" width="50%" />
<img src="https://github.com/user-attachments/assets/43f07c1b-1899-4fca-b856-494a246ada31" width="50%"/>

# Indicators
---

Green LED on: 3.3V regulation is working (PGOOD)

Red LED on: Gate driver fault detected (DRV_NFAULT)

# Electrical Setup Complete

Once done, you can move onto initial flashing of the board in [Firmware Setup](Firmware-setup.md).



