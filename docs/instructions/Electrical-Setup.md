Electrical-setup

--- Considerations ---
* No onboard OCP. It is recommended to use an offboard Slo-Blo fuse rated for 58V, 30-50A depending on usage parameters.
* No onboard reverse polarity protection, check XT60 connector orientation carefully before connecting.
* Board is rated for 6-50V, 2S-12S LiPo
* Inrush may occur if not using switch during LiPo battery connection.
* Be very careful with regeneration if using a PSU, an external regen clamp or implementation of flux braking in firmware is recommended before regen applications.

--- Indicators ---
Green LED on: 3.3V regulation is working (PGOOD)
Red LED on: Gate driver fault detected (DRV_NFAULT)

--- STM32 Hardware Setup ---
1. Initial Setup: SWD


<img width="374" height="317" alt="Screenshot 2026-09-02 at 3 56 35 PM" src="https://github.com/user-attachments/assets/29a40d64-4829-4905-839f-94ab0b0469a0" />


2. USB Setup (CDC, DFU)

4. CAN Setup (USB to SLCAN)



