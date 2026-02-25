/**
 * @file  drivers/i2c/backend/stm32h7/hal/inc/i2c_hal_backend.h
 * @brief STM32H7 HAL I2C backend skeleton.
 *
 * This backend wraps the STM32 HAL library (stm32h7xx_hal_i2c.h).  HAL calls
 * (HAL_I2C_Init, HAL_I2C_Master_Transmit, HAL_I2C_Master_Receive, …) are
 * confined to the companion implementation file; no HAL symbols leak into
 * portable layers.
 *
 * Hybrid-backend pattern
 * ----------------------
 * HalBackend may delegate low-level register access to CmsisBackend for
 * operations where direct register manipulation is preferred over the HAL API:
 *
 * @code
 * #include "drivers/i2c/backend/stm32h7/cmsis/inc/i2c_cmsis_backend.h"
 *
 * // Inside HalBackend::write():
 * using Helper = CmsisBackend<Periph, Policy, Wait>;
 * // Use CMSIS helper for timing-critical register manipulation …
 * @endcode
 *
 * IRQ glue
 * --------
 * See drivers/i2c/backend/stm32h7/hal/src/i2c_hal_irq_glue.cpp (placeholder).
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
namespace hal {

/// STM32H7 HAL I2C backend.
///
/// Provides the static interface required by i2c::Bus<Backend, Policy>.
template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
struct HalBackend {
    /// Initialise via HAL_I2C_Init() and configure GPIO with HAL_GPIO_Init().
    static void init();

    /// Release resources via HAL_I2C_DeInit().
    static void deinit();

    /// Blocking write via HAL_I2C_Master_Transmit().
    static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                       uint32_t timeout_ms);

    /// Blocking read via HAL_I2C_Master_Receive().
    static Error read(uint16_t addr, uint8_t* buf, std::size_t len,
                      uint32_t timeout_ms);

    /// Repeated-START write-then-read via HAL_I2C_Mem_Read().
    static Error write_read(uint16_t addr,
                            const uint8_t* tx_buf, std::size_t tx_len,
                            uint8_t*       rx_buf, std::size_t rx_len,
                            uint32_t       timeout_ms);
};

}  // namespace hal
}  // namespace stm32h7
}  // namespace backend
}  // namespace i2c
