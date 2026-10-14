---
title: YVGUI Instructions
parent: CAN Setup + GUI
nav_order: 1
---


# YVGUI 1.0 CAN Debugger Instructions
 
This program is a debugger over CAN; therefore the ST-Link is not required. On V1.0 firmware, this should run without any further firmware tweaks.

## Install Instructions

Install once: `python -m pip install python-can pyserial`

Within ```YVCAN1p0.py```, configure the following:

`INTERFACE`: `"slcan"` for an adapter with a COM port, `"gs_usb"` if using CandleLight.

`CHANNEL`: COM port number (0 for gs_usb)

`BITRATE`: `500000`

`NODE`: the board's node ID, `1` for one board.

Only one program can use the adapter at a time.
 
 
## Preprogrammed Protocols
These take the form of buttons.
 
| Button | Function |
| --- | --- |
| E-stop / Esc | Sets `drv_on = 0` as a priority, turning off all MOSFETs. |
| Driver on | Sets `drv_on = 1` |
| Calibrate offsets | `drv_on = 1`, `foc_mode = 0`, `ol_mode = 1`, `ol_v = 0`, `pwm_ms = 2000`, `cal_req = 1`, then starts a run |
| Align encoder | Holds the rotor at 0 degrees with `ol_v = 0.25` for 5 s, sets `align_req = 1` after 2.5 s |
| Save calibration | Sets `save_req = 1`. Refused while a run is active |
| Zero position | Sets `zero_req = 1`, so `p_rel` reads 0 at the current rotor angle |
| Clear fault | Writes `foc_fault = 0` |
| Start run | Writes `pwm_on = 1` |
 
**Status line:** connection state, refresh rate in Hz, `pwm_run` and `foc_fault`. It also reports a table mismatch, a write the board did not acknowledge, or an e-stop that was not acknowledged.
 
**Variable grid:** green values are writable and have an entry box and a "Set" button; grey values are read-only measurements. To change a value, type it in the box and press Enter or "set". A dash means the board did not answer that read.
 
 
## First Power-on
 
1. Wait about 2000ms, then check `cal_req = 0`, `off0`–`off2` between 1700 and 2400 (normally about 2050), `pwm_ms = 2000`.
2. Check `cal_ok = 1`, `e_off`, `pole_pairs = 20`, `foc_fault = 0`.
3. If any check fails, use the procedure below that fixes it before running FOC.

## Calibration Procedure

**Recalibrate current offsets:** 

click **Calibrate offsets**. `cal_req` returns to 0 and `off0`–`off2` should update.
 
**Align and save the encoder**
 
1. Click **Align encoder** and wait 3 s. `e_off` updates and `e_deg` reads about 0.
2. Check: `foc_mode = 0`, `ol_mode = 1`, `ol_v = 0.25`, `ol_rate = 0`, `ol_deg = 90`, **Start run**. `e_deg` should read 90 ± 10.
3. When the run has ended (`pwm_run = 0`), click **Save calibration** and check `save_res = 0`.
**Torque test:** `foc_mode = 1`, `ctrl_mode = 0`, `id_ref = 0`, `i_max = 1`, `iq_ref = 0.5`, `pwm_ms = 500`, **Start run**. The rotor turns in the direction where `ang` increases. With no load it accelerates to full speed, so keep runs short.
 

## Variables: run control, modes, calibration
 
Different settings to run FOC. W = write permitted, R = read-only.
 
| Variable | W/R | Units | Description | Typical |
| --- | --- | --- | --- | --- |
| `drv_on` | W | 0/1 | Driver on = 1, 0 = stop | 1 |
| `pwm_on` | W | 0/1 | Official run-starter, if set to 1. | 0 |
| `pwm_ms` | W | ms | Length of each run (ms) | user setting |
| `pwm_run` | R | 0/1 | 1 during run, sets itself to 0 when stops | User setting |
| `foc_mode` | W | 0/1 | 1 = FOC current loop controls PWM | User setting |
| `ctrl_mode` | W | 0/1/2 | Requires FOC=1: 0 = torque, 1 = velocity, 2 = position/impedance | User setting |
| `ol_mode` | W | 0/1 | 1 = open loop, setting FOC to 1 overrides. | 1 |
| `ol_v` | W | V | Open-loop vector amplitude: 0.1 V ≈ 1 A | 0 |
| `ol_deg` | W | electrical deg | Open-loop angle (vector angle) | 0 |
| `ol_rate` | W | deg per loop | Open-loop rate 0.36 ≈ 1 electrical rps | 0 |
| `cal_req` | W | 0/1 | Request for current-offset calibration. Required: `foc_mode = 0`, `ol_mode = 1`, `ol_v = 0` | 0 |
| `align_req` | W | 0/1 | Logs `e_off` during an open-loop hold at 0 deg | 0 |
| `save_req` | W | 0/1 | Saves `e_off` and `pole_pairs` to flash | 0 |
| `save_res` | R | code | Result of the last alignment save 0 = success; −1 run active; −2,−3,−4 error | 0 |
| `cal_ok` | R | 0/1 | 1 = Alignment from flash success | 1 |
| `zero_req` | W | 0/1 | Sets `p_rel` to zero | 0 |
| `foc_fault` | W | code | 0 = none, 1 = overcurrent, 2 = overspeed, 3 = sensing fault | 0 |
 
## Variables: controller settings
 
Mode selection, commanded references, and gains. FOC mode 0 = torque, 1 = velocity, 2 = position
 
| Variable | Units | Mode | Description | Typical |
| --- | --- | --- | --- | --- |
| `id_ref` | A | FOC | d-axis current | 0 |
| `iq_ref` | A | torque | q-axis current command | Commanded |
| `i_max` | A | FOC | Commanded current limit | Commanded |
| `i_trip` | A | FOC | Instant shutdown if any measured phase current exceeds it | 8 |
| `kp` | ~| FOC | Proportional gain for current loop PI | 0.3 |
| `ki_ts` | ~ | FOC | Current-loop integral gain | 0.03 |
| `vel_ref` | rad/s | velocity | Target speed | Commanded |
| `vel_kp` | A per rad/s | velocity | Velocity proportional gain, noise | 0.2 |
| `vel_ki` | A per rad | velocity | Velocity integral gain, smooths cogging | 15 |
| `p_ref` | rad | position | Target position relative to zero for position control | Commanded |
| `v_ff` | rad/s | position | Feed-forward velocity | 0 |
| `t_ff` | N·m | position | Feed forward torque | 0 |
| `kp_p` | ~ | position | Position kp, stiffness | 0.05–1 |
| `kd_p` | ~ | position | Position kd, damping| 0.002–0.01 |
| `e_off` | electrical deg | FOC | Encoder alignment offset, read only | Saved value |
| `pole_pairs` | Integer | FOC | Motor pole pairs | Depends on motor |
 
## Variables: measurements and diagnostics
 
All read-only.
 
| Variable | Units | Description | Healthy |
| --- | --- | --- | --- |
| `id`, `iq` | A | Measured d and q-axis currents | Follow `id_ref` and the active `iq` command |
| `vd`, `vq` | V | Current-loop output voltages | Not volatile |
| `ia`, `ib`, `ic` | A | Measured phase currents | Sum ~= 0 |
| `e_deg` | electrical degrees | Electrical angle used by FOC | Within 0-20 degrees of `ol_deg` |
| `p_rel` | rad | Relative position (multiple turns) | Regards full turns |
| `vel` | rad/s | Mechanical speed | Matches mechanical behabior |
| `ang` | counts (°) | Raw encoder angle 0–16383 | Test by hand, no jumps |
| `off0`–`off2` | ADC counts | Zero-current offsets of phases A, B, C | 1700–2400 |
| `isr_cycles` | CPU cycles | Time taken by the 20 kHz interrupt | <8500 |
| `nf_adc` | ADC counts | DRV nFAULT line via the ADC | ~2200 no fault; 0 = fault|
| `vdda_mv` | mV | Analog voltage supply (VDDA) | About 3300 |
| `par_err`, `ef_cnt` | count | Encoder parity error | Not increasing |
| `can_timeout_cnt` | count | CAN command timeout indicator | Not increasing |
| `rst_csr` | text | Previous reset diagnosis | Reset message |
 
## Safety rules and troubleshooting
 
Check the offsets and alignment before every FOC attempt.
 
- E-stop with Esc or the red button; if it is not acknowledged, turn off the supply.
- Do not pause while `pwm_run = 1`.
- Torque mode with no load accelerates to full speed, I recommend keeping runs <5000ms.
- PSU limit at 2–3 A for FOC tests (*as of 10-14-26) and check temperatures during long runs.
- Only change `foc_mode` between runs.

| Problem | Likely Cause | Solution |
| --- | --- | --- |
| Status "No reply from the board" | Board unpowered, incorrect COM port or CAN node | Check power, `CHANNEL`, `NODE`, about $60 \Omega$ between CANH and CANL |
| Status "Variable table mismatch" | Firmware and dashboard tables differ | Make sure firmware and software tables match and have the same number of variables. |
| Values stay as dashes after a power cycle | Reset | Restart program |
| `pwm_ms` stuck at 200, `cal_req = 1` | Start-up calibration unsuccessful | Click **Calibrate offsets**, set `pwm_ms = 2000` |
| `off0`–`off2` near 0 | VREF joint or VDDA lost | Check if VDDA and VREF is 3.3V, bodge resolder |
| Infinite spin even with `iq_ref = 0` | Current sensing not working | Check offsets; open-loop hold @ `ol_v = 0.12` should give `ia` about 1A |
| Jerks/stops | `e_off` wrong | Align, check at 90°, save |
| Turns the wrong way with + `iq_ref` | Angle flipped about 180° | Realign; check `pole_pairs = 20` |
| Board resets, `rst_csr` = brown-out | Supply cut: current limit or short | Raise current limit or check bodge |
| `foc_fault = 1` | Overcurrent trip | Decrease current, clear fault |
| `foc_fault = 2` | Overspeed trip | Adjust speed limit or current, clear fault |
