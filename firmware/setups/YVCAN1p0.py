"""
YVCAN Dashboard V1.0
Current: basic open loop and FOC control, torque, velocity, and position with impedance control

For emergency stop: press ESC

python -m pip install python-can pyserial
python dashboard.py
"""
import struct
import threading
import queue
import time
import tkinter as tk
from tkinter import ttk

import can

# settings
INTERFACE = "slcan" # If UART/COM port: "slcan" / if candlelight/native CAN: "gs_usb"
CHANNEL   = "COM7"  # If SLCAN: "COMx", if gs_usb: 0
BITRATE   = 500000
NODE      = 1
POLL_HZ   = 10 

REQ_ID, RSP_ID = 0x600 + NODE, 0x580 + NODE
F, U, I = 0, 1, 2       # float, uint32, int32
RO, RW = 0, 1

# Firmware references
VARS = [
    ("drv_on", U, RW), ("pwm_on", U, RW), ("pwm_ms", U, RW), ("pwm_run", U, RO),
    ("foc_mode", U, RW), ("ctrl_mode", U, RW), ("ol_mode", U, RW),
    ("ol_v", F, RW), ("ol_deg", F, RW), ("ol_rate", F, RW),
    ("cal_req", U, RW), ("align_req", U, RW), ("save_req", U, RW),
    ("save_res", I, RO), ("cal_ok", U, RO), ("zero_req", U, RW), ("foc_fault", U, RW),
    ("id_ref", F, RW), ("iq_ref", F, RW), ("i_max", F, RW), ("i_trip", F, RW),
    ("kp", F, RW), ("ki_ts", F, RW),
    ("vel_ref", F, RW), ("vel_kp", F, RW), ("vel_ki", F, RW),
    ("p_ref", F, RW), ("v_ff", F, RW), ("t_ff", F, RW), ("kp_p", F, RW), ("kd_p", F, RW),
    ("e_off", F, RW), ("pole_pairs", F, RW),
    ("id", F, RO), ("iq", F, RO), ("vd", F, RO), ("vq", F, RO),
    ("ia", F, RO), ("ib", F, RO), ("ic", F, RO),
    ("e_deg", F, RO), ("p_rel", F, RO), ("vel", F, RO),
    ("off0", I, RO), ("off1", I, RO), ("off2", I, RO),
    ("isr_cycles", U, RO), ("nf_adc", U, RO), ("vdda_mv", U, RO),
    ("par_err", U, RO), ("ef_cnt", U, RO), ("can_timeout_cnt", U, RO),
    ("ang", U, RO), ("rst_csr", U, RO),
]
IDX = {name: i for i, (name, _, _) in enumerate(VARS)}
FMT = {F: "<f", U: "<I", I: "<i"}

# Button sequences with second pauses
CALIBRATE = [("drv_on", 1), ("foc_mode", 0), ("ol_mode", 1), ("ol_v", 0.0),
             ("pwm_ms", 2000), ("cal_req", 1), ("pwm_on", 1)]
ALIGN = [("drv_on", 1), ("foc_mode", 0), ("ol_mode", 1), ("ol_rate", 0.0),
         ("ol_deg", 0.0), ("ol_v", 0.25), ("pwm_ms", 5000), ("pwm_on", 1),
         2.5, ("align_req", 1)]


def reset_cause(csr):
    flags = [(31, "low-power"), (30, "WWDG"), (29, "IWDG"), (28, "software"),
             (27, "brown-out"), (26, "reset pin"), (25, "option bytes")]
    hits = [n for b, n in flags if csr & (1 << b)]
    return ", ".join(hits) if hits else hex(csr)

# Variable table access class
class Link:
    

    def __init__(self):
        self.bus = can.Bus(interface=INTERFACE, channel=CHANNEL, bitrate=BITRATE)

    def xfer(self, op, idx, raw=b"\x00" * 4, timeout=0.05):
        self.bus.send(can.Message(arbitration_id=REQ_ID, is_extended_id=False,
                                  data=bytes([op, idx, 0, 0]) + raw))
        end = time.time() + timeout
        while True:
            left = end - time.time()
            if left <= 0:
                return None
            m = self.bus.recv(timeout=left)
            if m and m.arbitration_id == RSP_ID and len(m.data) == 8 and m.data[1] == idx:
                return bytes(m.data)

    def read(self, idx):
        d = self.xfer(1, idx)
        if not d or d[0] != 0x81:
            return None
        return struct.unpack(FMT[VARS[idx][1]], d[4:8])[0]

    def write(self, idx, value):
        t = VARS[idx][1]
        value = float(value) if t == F else int(float(value))
        d = self.xfer(2, idx, struct.pack(FMT[t], value))
        return bool(d and d[0] == 0x82)

    def count(self):
        d = self.xfer(3, 0)
        return d[4] if d and d[0] == 0x83 else None

# Working class
class Worker(threading.Thread): 

    def __init__(self, link):
        super().__init__(daemon=True)
        self.link = link
        self.writes = queue.Queue()
        self.estop = threading.Event()
        self.values = {}
        self.status = "Connecting..."
        self.rate = 0.0
        self.running = True

    def write(self, name, value):
        self.writes.put((name, value))

    def _service(self):
        if self.estop.is_set():
            self.estop.clear()
            ok = self.link.write(IDX["drv_on"], 0)
            self.status = "E-stop sent" if ok else "E-stop failed - turn off PSU"
        while not self.writes.empty():
            name, val = self.writes.get()
            if not self.link.write(IDX[name], val):
                self.status = f"Write to {name} was not acknowledged"

    def run(self):
        n = self.link.count()
        if n is None:
            self.status = "No board response - check power, wiring, COM port and NODE"
        elif n != len(VARS):
            self.status = f"Variable table mismatch: board has {n}, dashboard has {len(VARS)}"
        else:
            self.status = "Connected"
        period = 1.0 / POLL_HZ
        while self.running:
            t0 = time.time()
            for i, (name, _, _) in enumerate(VARS):
                if not self.running:
                    break
                self._service()
                self.values[name] = self.link.read(i)
            dt = time.time() - t0
            self.rate = 1.0 / dt if dt > 0 else 0.0
            if dt < period:
                time.sleep(period - dt)


def run_sequence(worker, steps):
    def go():
        for s in steps:
            if isinstance(s, (int, float)):
                time.sleep(s)
            else:
                worker.write(*s)
    threading.Thread(target=go, daemon=True).start()


class App(tk.Tk):
    def __init__(self, worker):
        super().__init__()
        self.w = worker
        self.title(f"YVCAN Dashboard V1.0 | Node {NODE}")

        bar = ttk.Frame(self, padding=6)
        bar.pack(fill="x")
        tk.Button(bar, text="STOP (Esc)", bg="#b71c1c", fg="white",
                  activebackground="#e53935", font=("Segoe UI", 11, "bold"),
                  command=self.do_estop).pack(side="left", padx=(0, 10))
        actions = [
            ("Driver on", lambda: worker.write("drv_on", 1)),
            ("Calibrate offsets", lambda: run_sequence(worker, CALIBRATE)),
            ("Align encoder", lambda: run_sequence(worker, ALIGN)),
            ("Save calibration", self.do_save),
            ("Zero position", lambda: worker.write("zero_req", 1)),
            ("Clear fault", lambda: worker.write("foc_fault", 0)),
            ("Start run", lambda: worker.write("pwm_on", 1)),
        ]
        for text, cmd in actions:
            ttk.Button(bar, text=text, command=cmd).pack(side="left", padx=2)
        self.bind("<Escape>", lambda e: self.do_estop())

        self.status = tk.StringVar()
        ttk.Label(self, textvariable=self.status, padding=(8, 2)).pack(fill="x")

        grid = ttk.Frame(self, padding=6)
        grid.pack(fill="both", expand=True)
        self.vals = {}
        half = (len(VARS) + 1) // 2
        for i, (name, t, rw) in enumerate(VARS):
            col = 0 if i < half else 4
            row = i if i < half else i - half
            ttk.Label(grid, text=name, width=15).grid(row=row, column=col, sticky="w")
            sv = tk.StringVar(value="-")
            self.vals[name] = sv
            ttk.Label(grid, textvariable=sv, width=18, anchor="e",
                      foreground="#1b5e20" if rw else "#555555").grid(row=row, column=col + 1, sticky="e")
            if rw:
                e = ttk.Entry(grid, width=10)
                e.grid(row=row, column=col + 2, padx=2)
                e.bind("<Return>", lambda ev, n=name, en=e: self.set_value(n, en))
                ttk.Button(grid, text="Set", width=4,
                           command=lambda n=name, en=e: self.set_value(n, en)).grid(
                    row=row, column=col + 3, padx=(0, 16))

        self.after(150, self.refresh)

    def set_value(self, name, entry):
        txt = entry.get().strip()
        if not txt:
            return
        try:
            float(txt)
        except ValueError:
            self.w.status = f"'{txt}' is not a number"
            return
        self.w.write(name, txt)
        entry.delete(0, "end")

    def do_estop(self):
        self.w.estop.set()

    def do_save(self):
        if self.w.values.get("pwm_run") == 0:
            self.w.write("save_req", 1)
            self.w.status = "Saving: check save_res (0 = saved)"
        else:
            self.w.status = "Stop the run before saving (pwm_run must be 0)"

    def refresh(self):
        for name, t, _ in VARS:
            v = self.w.values.get(name)
            if v is None:
                text = "-"
            elif name == "ang":
                text = f"{v}  ({v * 360.0 / 16384.0:6.1f}°)"
            elif name == "rst_csr":
                text = reset_cause(v)
            else:
                text = f"{v:.4g}" if t == F else str(v)
            self.vals[name].set(text)
        run, fault = self.w.values.get("pwm_run"), self.w.values.get("foc_fault")
        self.status.set(f"{self.w.status}     refresh {self.w.rate:4.1f} Hz     "
                        f"run {run}     fault {fault}")
        self.after(150, self.refresh)


if __name__ == "__main__":
    link = Link()
    worker = Worker(link)
    worker.start()
    app = App(worker)
    try:
        app.mainloop()
    finally:
        worker.running = False
        worker.join(timeout=1.0)
        try:
            link.write(IDX["drv_on"], 0) # turn off driver on exit
        except Exception:
            pass
        link.bus.shutdown()
