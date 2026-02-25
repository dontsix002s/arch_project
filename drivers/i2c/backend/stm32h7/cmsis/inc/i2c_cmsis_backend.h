/**
 * @file  drivers/i2c/backend/stm32h7/cmsis/inc/i2c_cmsis_backend.h
 * @brief STM32H7 CMSIS-level I2C backend skeleton.
 *
 * Manipulates I2C peripheral registers directly through CMSIS-Core register
 * macros (reads/writes to I2C_TypeDef fields).  No HAL or LL calls are made.
 *
 * This backend can be used either:
 *   a) Standalone – as the sole backend for projects that avoid HAL/LL, or
 *   b) As a helper ("hybrid-backend") – called from LlBackend or HalBackend
 *      for register-level operations that are inconvenient through the
 *      higher-level API.
 *
 * Hybrid-backend pattern (example)
 * ---------------------------------
 * @code
 * // Inside LlBackend<Periph, Policy, Wait>::write():
 * #include "drivers/i2c/backend/stm32h7/cmsis/inc/i2c_cmsis_backend.h"
 *
 * using Helper = CmsisBackend<Periph, Policy, Wait>;
 * Helper::set_nbytes(len);   // hypothetical register helper
 * @endcode
 *
 * IRQ glue
 * --------
 * See drivers/i2c/backend/stm32h7/cmsis/src/i2c_cmsis_irq_glue.cpp
 * (placeholder).
 *
 * @tparam Periph    A constexpr PeriphDescriptor for the target I2C instance.
 * @tparam Policy    Transfer policy tag (PollingPolicy | IrqPolicy |
 *                   DmaIrqPolicy).
 * @tparam Wait      Wait strategy (BaremetalWait | ThreadXWait).
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "drivers/i2c/core/inc/i2c_error.h"
#include "drivers/i2c/backend/stm32h7/common/inc/i2c_stm32h7_periph.h"

namespace i2c {
namespace backend {
namespace stm32h7 {
namespace cmsis {

/// STM32H7 CMSIS-level I2C backend.
///
/// Provides the static interface required by i2c::Bus<Backend, Policy>.
template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
struct CmsisBackend {
    /// Configure clocks and GPIO, then program timing/CR1/CR2 registers
    /// directly using CMSIS register macros.
    static void init();

    /// Disable peripheral; reset register fields to power-on defaults.
    static void deinit();

    /// Polling write driven entirely by register flags (no LL/HAL calls).
    static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                       uint32_t timeout_ms);

    /// Polling read driven entirely by register flags.
    static Error read(uint16_t addr, uint8_t* buf, std::size_t len,
                      uint32_t timeout_ms);

    /// Repeated-START write-then-read.
    static Error write_read(uint16_t addr,
                            const uint8_t* tx_buf, std::size_t tx_len,
                            uint8_t*       rx_buf, std::size_t rx_len,
                            uint32_t       timeout_ms);
};

}  // namespace cmsis
}  // namespace stm32h7
}  // namespace backend
}  // namespace i2c
