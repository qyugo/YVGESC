---
title: V1 Issues
parent: Issues
nav_order: 1
---

# V1 Issues

Five boards were manufactured for the first iteration of the YVGESC (YVGESC-1). A few issues to note:

## Issue 1. Encoder SPI Lines

The encoder was intended to wire in at SPI2 (PB12, 13, 14, 25) for CSN, CLK, MOSI, and MISO. PB14 to MOSI and PB15 to MISO was mistakenly wired in the schematic, so this will be changed for a V2 version of the board.

<div style="display: flex; gap: 1rem;">
  <img src="https://github.com/user-attachments/assets/096463d2-eb6a-43fe-9e80-67c9ddd461f6" style="width: 48%;">
  <img src="https://github.com/user-attachments/assets/97cb666a-ae78-44de-a773-1230b87d9f3a" style="width: 45%;">
</div>

This can be worked out in firmware, with more detailed instructions provided in Firmware Setup. The solution was as follows:

i. Instead of Full Duplex master, PB14->MOSI will need to be a permanently high GPIO and SPI2 will be configured in Half Duplex Master mode.

ii. Additionally, PB12 (ENC_CS) is set to HIGH GPIO. With NSS disabled, the pinout should show ENC_CS instead of SPI2_NSS.

iii. AS5047P encoders deliver 16 bits per frame and runs on a 2-Edge clock phase, so the setting must match accordingly.

SPI2 is then effectively configured only to receive. The functions ```HAL_SPI_Transmit``` and ```HAL_SPI_TransmitReceive``` therefore, should never be called.

## Issue 2. VDDA and VREF+ Hardware Jumpers

VDDA on YVGESC-V1.0 was grounded in hardware, will be fixed in V2. The GND-side route to GND via of FB1 should be cut, or FB1 can also be tombstoned on its VDDA side. The former GND side should be jumped to any 3.3V source, such as the VDD side of COUT or the VDD pin on the SWD, since the pin is unused for the ST-Link.

I made a mistake in the design and connected the ferrite bead input to the GND side of Pin 64 (VDD)'s decoupling capacitor, so basically FB1 is filtering signals from the shadow realm.

VDDA comprises a small island shared by FB1 and C14, C15, and C16. The ferrite bead acts as a filter-dependent frequency once the input side is actually 3.3V, so afterwards anything from VDDA should be a clean signal. This can be viewed in Figure 1, highlighted in red.

Further, VREF+ on the DRV8323H was unpowered, which slipped my check as well, since I had incorrectly assumed that the DRV8323H's onboard 3.3V LDO would go to VREF+, and decoupled it to GND via C6. Fortunately, C6 is a barely solderable joint (no need to jump from the tiny WDFN-40 pins. Make sure to jump a clean 3.3V (I used the VDDA side of C14, since it's the biggest of the VDDA capacitors and also a filtered 3.3 signal post-ferrite), to the VREF side of C6. Image attached below in Figure 2.


<figure style="margin: 0;">
  <div style="display: flex; gap: 1rem;">
    <img src="https://github.com/user-attachments/assets/0765778f-56ae-4f66-b695-4165419f1750" style="width: 48%;">
    <img src="https://github.com/user-attachments/assets/0e6e575f-09ab-4b11-9c28-83a12f86289e" style="width: 40%;">
  </div>
  <figcaption style="text-align: center; margin-top: 0.5rem;">
    Figure 1 (left): VDDA island and sever mark. <br> Figure 2 (right): VREF+, VDDA, and VDD jumper configuration.
  </figcaption>
</figure>


