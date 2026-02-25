# arch_project – STM32 Driver Architecture Template

A **maximally clean, layered** embedded driver architecture for STM32
microcontrollers, starting with I2C.  Every concern lives in exactly one
directory.  Backends, policies, and wait strategies are **swapped by changing a
single type alias**; no application or device-driver code changes.

---

## Directory Structure

```
arch_project/
│
├── drivers/i2c/                        # I2C driver module
│   ├── core/inc/                       # Portable, dependency-free interfaces
│   │   ├── i2c_types.h                 #   AddressMode, Speed, Direction
│   │   ├── i2c_error.h                 #   i2c::Error enumeration
│   │   ├── i2c_traits.h                #   i2c::Traits – backend contract
│   │   ├── i2c_context.h               #   TransferContext (for IRQ/DMA modes)
│   │   └── i2c_bus.h                   #   i2c::Bus<Backend, Policy> façade
│   │
│   ├── policy/inc/                     # Transfer-mode compile-time tags
│   │   ├── i2c_policy_polling.h        #   Blocking busy-wait
│   │   ├── i2c_policy_irq.h            #   Interrupt-driven
│   │   └── i2c_policy_dma_irq.h        #   DMA + interrupt
│   │
│   ├── wait/inc/                       # Synchronisation strategies
│   │   ├── i2c_wait_baremetal.h        #   Spinning loop (no RTOS)
│   │   └── i2c_wait_threadx.h          #   Azure RTOS ThreadX semaphore
│   │
│   └── backend/stm32h7/
│       ├── common/inc/
│       │   └── i2c_stm32h7_periph.h    #   PeriphDescriptor + handler slots
│       │
│       ├── ll/                         #   STM32 Low-Level (LL) backend
│       │   ├── inc/i2c_ll_backend.h
│       │   └── src/i2c_ll_irq_glue.cpp #   (placeholder) IRQ wiring
│       │
│       ├── hal/                        #   STM32 HAL backend
│       │   ├── inc/i2c_hal_backend.h
│       │   └── src/i2c_hal_irq_glue.cpp
│       │
│       └── cmsis/                      #   CMSIS register-level backend
│           ├── inc/i2c_cmsis_backend.h
│           └── src/i2c_cmsis_irq_glue.cpp
│
├── board/rev_a/inc/                    # Board revision A configuration
│   ├── board_config.h                  #   PeriphDescriptor constants + Bus aliases
│   └── pin_map.h                       #   GPIO pin assignments
│
├── devices/ft6236/inc/                 # External device drivers
│   └── ft6236.h                        #   FT6236 capacitive touch controller
│
├── app/                                # Application layer
│   ├── inc/app_config.h                #   Compile-time tunables
│   └── src/main.cpp                    #   Entry point
│
├── system/                             # Early MCU initialisation
│   ├── inc/system_init.h
│   └── src/system_init.cpp
│
├── CMakeLists.txt                      # CMake build description
└── README.md
```

All `#include` directives use the **full path from the repository root**:

```cpp
#include "drivers/i2c/core/inc/i2c_bus.h"
#include "drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h"
#include "board/rev_a/inc/board_config.h"
```

---

## Layering Rules and Contracts

### Layer dependency graph

```
app / system
    │
    ▼
board/rev_a          ← knows which backend + policy + wait to use
    │
    ▼
drivers/i2c/backend/stm32h7/<type>    ← MCU + backend specific
    │
    ├── drivers/i2c/core              ← portable: types, traits, bus façade
    ├── drivers/i2c/policy            ← portable: policy tags
    └── drivers/i2c/wait              ← portable: wait strategies

devices/<name>       ← depends only on drivers/i2c/core (the bus façade)
```

**Rules:**

| Source layer | May include | Must NOT include |
|---|---|---|
| `drivers/i2c/core` | `<cstdint>`, `<cstddef>` | anything else |
| `drivers/i2c/policy` | `core` | `backend`, `board`, `devices` |
| `drivers/i2c/wait` | `<cstdint>` | `backend`, `board`, `devices`, `policy` |
| `drivers/i2c/backend/stm32h7/*` | `core`, `policy`, `wait`, vendor SDK | `board`, `devices`, `app` |
| `board/*` | any `drivers/i2c/*`, `<cstdint>` | `devices`, `app` |
| `devices/*` | `drivers/i2c/core` | `backend`, `board`, `app` |
| `app/*` | everything above | – |

### Backend contract (`i2c::Traits`)

Every backend must expose these **static methods** (see
`drivers/i2c/core/inc/i2c_traits.h`):

```cpp
static void  init();
static void  deinit();
static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                   uint32_t timeout_ms);
static Error read (uint16_t addr, uint8_t* buf,  std::size_t len,
                   uint32_t timeout_ms);
static Error write_read(uint16_t addr,
                        const uint8_t* tx_buf, std::size_t tx_len,
                        uint8_t*       rx_buf, std::size_t rx_len,
                        uint32_t       timeout_ms);
```

### PeriphDescriptor handler slots

The `PeriphDescriptor` struct
(`drivers/i2c/backend/stm32h7/common/inc/i2c_stm32h7_periph.h`) carries four
`HandlerFn` (= `void(*)()`) slots:

| Slot | Assigned by | Called from |
|---|---|---|
| `IrqEventHandler` | `Backend::init()` | `I2Cx_EV_IRQHandler` |
| `IrqErrorHandler` | `Backend::init()` | `I2Cx_ER_IRQHandler` |
| `DmaTxHandler`    | `Backend::init()` | DMA Tx stream IRQHandler |
| `DmaRxHandler`    | `Backend::init()` | DMA Rx stream IRQHandler |

Slots left `nullptr` are silently ignored.  This means a polling backend
requires no IRQ glue at all; a DMA+IRQ backend populates all four.

---

## How to Add Another Driver (e.g. SPI or UART)

Follow exactly the same pattern as I2C:

1. **Create the module directory tree:**
   ```
   drivers/spi/core/inc/spi_traits.h
   drivers/spi/core/inc/spi_error.h
   drivers/spi/core/inc/spi_bus.h
   drivers/spi/policy/inc/spi_policy_polling.h
   drivers/spi/policy/inc/spi_policy_dma_irq.h
   drivers/spi/wait/inc/spi_wait_baremetal.h
   drivers/spi/backend/stm32h7/common/inc/spi_stm32h7_periph.h
   drivers/spi/backend/stm32h7/ll/inc/spi_ll_backend.h
   drivers/spi/backend/stm32h7/ll/src/spi_ll_irq_glue.cpp
   ```

2. **Define `spi::Traits`** in `spi_traits.h` with the same static-method
   pattern (init / deinit / transfer / …).

3. **Define `spi::Bus<Backend, Policy>`** in `spi_bus.h` – identical structure
   to `i2c::Bus`.

4. **Add a `PeriphDescriptor`** constant in `board/rev_a/inc/board_config.h`
   with the SPI1 base address and handler slots.

5. **Add a `SpixBus` alias** in `board_config.h`.

6. **Wire device drivers** (e.g. `devices/ili9341/inc/ili9341.h`) to accept
   any `Bus` type as a template parameter – just like `Ft6236<Bus>`.

7. **Register the new CMake targets** in `CMakeLists.txt` following the
   existing `i2c_stm32h7_ll` / `board_rev_a` pattern.

**Nothing outside the new files changes.** Existing I2C code is unaffected.

---

## Hybrid Backends (HAL+CMSIS, LL+CMSIS)

A *hybrid backend* uses more than one vendor API in the same translation unit.
Common reasons:

* The LL library handles clock and GPIO init conveniently, but you want direct
  register access for the transfer loop (latency-critical).
* The HAL library provides a well-tested `HAL_I2C_Init()` but you need
  register-level control for a custom repeated-START sequence.

### LL + CMSIS example

```cpp
// Inside LlBackend<Periph, Policy, Wait>::write():
#include "drivers/i2c/backend/stm32h7/cmsis/inc/i2c_cmsis_backend.h"

// Use LL for init (in LlBackend::init()):
//   LL_I2C_Enable(reinterpret_cast<I2C_TypeDef*>(Periph.base_address));

// Use CMSIS register macros for the byte loop:
//   auto* i2c = reinterpret_cast<I2C_TypeDef*>(Periph.base_address);
//   while (!(i2c->ISR & I2C_ISR_TXE)) { /* wait */ }
//   i2c->TXDR = *buf++;
```

### HAL + CMSIS example

```cpp
// Inside HalBackend<Periph, Policy, Wait>::init():
//   HAL_I2C_Init(&hi2c1);   // let HAL manage the I2C_HandleTypeDef state

// Inside HalBackend<…>::write():
//   // For a non-standard STOP condition timing, bypass HAL:
//   auto* i2c = reinterpret_cast<I2C_TypeDef*>(Periph.base_address);
//   i2c->CR2 |= I2C_CR2_STOP;
```

**Key principle:** vendor API calls stay inside the backend `.cpp` file.  The
`core`, `policy`, `wait`, `board`, and `device` layers never see HAL, LL, or
CMSIS symbols.

---

## Building

```bash
cmake -S . -B build -DCMAKE_CXX_STANDARD=17
cmake --build build
```

> **Note:** The `app` executable target will not fully link until the backend
> method bodies (e.g. `LlBackend::init()`) are implemented.  It is provided as
> a compile-time correctness check for the scaffolding headers.

---

## Compiler Requirements

* C++17 or later.
* Standard-library headers only (`<cstdint>`, `<cstddef>`).  No STM32 vendor
  headers are included in the scaffolding; add them inside the backend `.cpp`
  files when implementing the driver bodies.
