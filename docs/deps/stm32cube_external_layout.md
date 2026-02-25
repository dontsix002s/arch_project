# STM32Cube External Vendor Layout

This document describes the standardised folder layout for STM32Cube vendor
libraries used by this project, the required preprocessor defines, and the
include paths required to compile the LL backend.

> **Important:** Vendor headers must remain **confined to backend `.cpp`
> translation units**.  Portable layers (`drivers/i2c/core`, `policy`, `wait`,
> `devices`, `board`) must never include any STM32 vendor header.  This
> constraint is enforced by architecture convention and should be verified by
> code-review.

---

## 1. External folder layout

All vendor files live under `external/` at the repository root.  Use
**lowercase** folder names throughout.

```
external/
├── cmsis/
│   └── inc/
│       ├── stm32h7xx.h                   # top-level family header (includes MCU-specific)
│       ├── stm32h743xx.h                 # MCU-specific register map
│       ├── stm32h750xx.h                 # MCU-specific register map (debug target)
│       ├── core_cm7.h                    # Cortex-M7 core peripheral access
│       ├── cmsis_compiler.h
│       ├── cmsis_gcc.h                   # (or cmsis_armcc.h / cmsis_iar.h)
│       └── system_stm32h7xx.h
│
├── hal/
│   ├── inc/
│   │   ├── stm32h7xx_hal.h
│   │   ├── stm32h7xx_hal_conf.h          # HAL configuration (copy & edit template)
│   │   ├── stm32h7xx_hal_i2c.h
│   │   └── …
│   └── src/
│       ├── stm32h7xx_hal.c
│       ├── stm32h7xx_hal_i2c.c
│       └── …
│
└── ll/
    ├── inc/
    │   ├── stm32h7xx_ll_bus.h
    │   ├── stm32h7xx_ll_gpio.h
    │   ├── stm32h7xx_ll_i2c.h
    │   ├── stm32h7xx_ll_rcc.h
    │   └── …
    └── src/
        └── (LL drivers are header-only; src/ reserved for future use)
```

> **Not committed.**  Vendor files are distributed by STMicroelectronics under
> their own licence and must not be committed to this repository.  Obtain them
> from [STM32CubeH7](https://github.com/STMicroelectronics/STM32CubeH7) or via
> STM32CubeMX code generation and place them under `external/` locally.

---

## 2. Required preprocessor defines

Define exactly **one** MCU symbol to select the correct register-map header
inside `stm32h7xx.h`.  Set this define in your IDE / build system (not in
source code).

| Debug / build target | Define to add |
|---|---|
| STM32H750xB (8-pin WLCSP debug board) | `STM32H750xx` |
| STM32H743ZI (Nucleo-H743ZI) | `STM32H743xx` |
| STM32H743VI | `STM32H743xx` |

### VisualGDB / Visual Studio

1. Open **Project Properties → C/C++ → Preprocessor → Preprocessor Definitions**.
2. Add `STM32H750xx` (or the appropriate symbol for your target).
3. Optionally add `USE_FULL_LL_DRIVER` to enable the LL inline function bodies
   (required if you use LL functions that are not always-inline macros).

### CMake (bare-metal / arm-none-eabi toolchain)

```cmake
target_compile_definitions(i2c_stm32h7_ll_impl PRIVATE
    STM32H750xx
    USE_FULL_LL_DRIVER
)
```

---

## 3. Include directories for LL backend compilation

The translation unit
`drivers/i2c/backend/stm32h7/ll/src/i2c_ll_backend.cpp`
needs the following include paths (in addition to the repository root):

| Path | Purpose |
|---|---|
| `external/cmsis/inc` | `stm32h7xx.h`, CMSIS core headers |
| `external/ll/inc` | `stm32h7xx_ll_i2c.h`, `stm32h7xx_ll_gpio.h`, etc. |

No other backend `.cpp` files currently require additional include paths.

### VisualGDB / Visual Studio – Additional Include Directories

Add the following under **Project Properties → C/C++ → General → Additional
Include Directories**:

```
$(SolutionDir)external\cmsis\inc
$(SolutionDir)external\ll\inc
```

---

## 4. Vendor header confinement rule

```
┌──────────────────────────────────────────────┐
│  portable layers                             │
│  (core / policy / wait / devices / board)    │
│                                              │
│  ✗  must NOT include stm32h7xx*.h            │
│  ✗  must NOT include stm32h7xx_ll_*.h        │
│  ✗  must NOT include stm32h7xx_hal_*.h       │
└────────────────────┬─────────────────────────┘
                     │ only via i2c::Bus<> template
                     ▼
┌──────────────────────────────────────────────┐
│  backend .cpp translation units              │
│  (e.g. i2c_ll_backend.cpp)                  │
│                                              │
│  ✔  may include stm32h7xx.h                 │
│  ✔  may include stm32h7xx_ll_*.h            │
│  ✔  may include stm32h7xx_hal_*.h           │
└──────────────────────────────────────────────┘
```

Keeping vendor headers out of portable layers ensures that:
- Unit tests for core logic can run on the host without a cross-compiler.
- Switching to a different STM32 family or backend (HAL, CMSIS, RTOS-aware)
  requires changes only inside the backend `.cpp`.

---

## 5. TIMINGR placeholder

`i2c_ll_backend.cpp` hard-codes the constant `kTimingr100kHz = 0x10C0ECFFU`
as a placeholder for 100 kHz Standard Mode at a 100 MHz I2C kernel clock.
Recalculate this value with STM32CubeMX or AN4235 once the actual clock-tree
configuration is finalised.
