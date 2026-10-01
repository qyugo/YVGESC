## Blink test on PB11 (33), toggle 2000ms

Here is a quick setup for flashing a 0-3.3v blink on the PB11 pin, since I ran out of LEDs.

## YVGESC Blinker

Initial testing firmware is configured in STM32CubeMX.

From the Pinout and Configuration tab on the home page, switch to System View and configure the following:

SYS: Debug -> Serial Wire

<img width="503" height="310" alt="image" src="https://github.com/user-attachments/assets/39994454-9a9f-47b6-8d1e-ab522155912b" />

<img width="506" height="309" alt="image" src="https://github.com/user-attachments/assets/c5a6e27e-7244-4917-80a7-e17a1454a782" />

The oscillator that this board uses is an Abracon AB8MG. From the Clock tab, set input frequency to 8MHz, and HSE at System Clock Mux and PLL Source Mux.

<img width="1413" height="686" alt="image" src="https://github.com/user-attachments/assets/98adffba-1833-4f0f-a192-1a2663f76caf" />

DRV_ENABLE

The MCU on the YVGESC maps PC0 (8) to DRV_ENABLE. In Pinout view, this is set to GPIO output, ensuring output level: LOW in System Core -> GPIO.

<img width="604" height="643" alt="image" src="https://github.com/user-attachments/assets/7295d300-2313-416a-9930-deacb20ec107" />

Timer toggle

<img width="517" height="559" alt="image" src="https://github.com/user-attachments/assets/ed1acf41-6bb0-4f73-9f0c-f7ca3b9a1fca" />

<img width="1267" height="770" alt="image" src="https://github.com/user-attachments/assets/64430369-78c6-42a8-ac9f-762ce62374bc" />

