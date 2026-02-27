# SWO/ITM Logger – VisualGDB Setup

This document describes how to configure VisualGDB to receive SWO output
from the STM32H750VB using either a J-Link or ST-Link v2.1 probe.

---

## Prerequisites

- VisualGDB 5.6 or later installed in Visual Studio
- J-Link (recommended) **or** ST-Link v2.1 with SWO pin wired to the target
- `stmfw::system::swo::init()` called in `main()` **after** `stmfw::board::init()`

---

## 1. Wire SWO

| Target pin | Probe connection |
|-----------|-----------------|
| SWO (PB3 on STM32H750VB) | J-Link pin 13 / ST-Link SWO |

---

## 2. Enable SWO in VisualGDB project settings

1. Right-click project → **VisualGDB Project Properties**.
2. Go to **Debug Settings → SWO and ITM**.
3. Enable **SWO output**.
4. Set **SWO clock** to `2000000` (2 MHz – must match `k_swo_baud` in
   `system/src/system_swo.cpp`).
5. Set **CPU clock** to `480000000` (480 MHz – must match the argument passed
   to `stmfw::system::swo::init()`).

---

## 3. Open the ITM console

1. **Debug** → **Windows** → **VisualGDB** → **ITM console**.
2. Ensure **port 0** is checked/enabled.
3. Start a debug session – "boot" and "board init ok" should appear
   immediately after the target reaches the super-loop.

---

## 4. Why `swo::init()` must follow `stmfw::board::init()`

`stmfw::board::init()` configures the PLL and switches the CPU to the
Performance-profile clock (480 MHz).  `TPI->ACPR` (the SWO baud prescaler)
is derived from the live CPU frequency, so computing it before the final
clock configuration would yield an incorrect baud rate and garbled output.

Always preserve this order in `main()`:

```cpp
stmfw::system::init();           // FPU, MPU, caches
stmfw::board::init();            // clock tree → 480 MHz, TIM7 timebase
stmfw::system::swo::init(480'000'000U);  // SWO prescaler based on stable clock
stmfw::system::swo::write("boot\n");
```

---

## 5. Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| No output in ITM console | SWO not wired / wrong pin | Check SWO wire to PB3 |
| Garbled characters | Speed mismatch | Ensure VisualGDB SWO clock = 2 000 000 and `cpu_hz` arg = 480 000 000 |
| No output with ST-Link | Older ST-Link firmware | Update ST-Link firmware via STM32CubeProgrammer |
| Output stops after reset | `swo::init()` not called | Check `main()` calls `stmfw::system::swo::init()` after `stmfw::board::init()` |
