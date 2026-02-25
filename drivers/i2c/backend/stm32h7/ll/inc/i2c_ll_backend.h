/**
 * @file  drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h
 * @brief STM32H7 Low-Level (LL) I2C backend skeleton.
 *
 * This backend drives the I2C peripheral through the STM32 LL library API
 * (stm32h7xx_ll_i2c.h).  All LL API calls are confined to the companion
 * implementation file; no LL symbols appear in portable layers.
 *
 * Hybrid-backend pattern
 * ----------------------
 * LlBackend may delegate register-level operations to CmsisBackend for
 * fine-grained control while still using LL for clock and GPIO initialisation:
 *
 * @code
 * #include "drivers/i2c/backend/stm32h7/cmsis/inc/i2c_cmsis_backend.h"
 *
 * // Inside LlBackend::write():
 * using Helper = CmsisBackend<Periph, Policy, Wait>;
 * // Call CMSIS helpers for timing-critical register manipulation …
 * @endcode
 *
 * IRQ glue
 * --------
 * See drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp (placeholder)
 * for the companion file that wires up the STM32 IRQ handler functions and
 * populates Periph.IrqEventHandler / IrqErrorHandler / DmaTxHandler /
 * DmaRxHandler.
 *
 * Extension point
 * ---------------
 * To port to another STM32 family (e.g. STM32L4):
 *   1. Copy this directory tree to drivers/i2c/backend/stm32l4/ll/.
 *   2. Replace LL API calls in the .cpp file with the L4 LL equivalents.
 *   3. Provide a new PeriphDescriptor for the target peripheral in the board
 *      config.
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
namespace ll {

/// STM32H7 LL I2C backend.
///
/// Provides the static interface required by i2c::Bus<Backend, Policy>.
/// Method bodies live in the companion .cpp file (not yet implemented).
template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
struct LlBackend {
    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------

    /// Configure GPIO pins, enable peripheral clock, set timing registers,
    /// and call LL_I2C_Init() with the speed derived from Periph.clock_hz.
    static void init();

    /// Disable the peripheral and release GPIO pins.
    static void deinit();

    // -------------------------------------------------------------------------
    // Transfers
    // -------------------------------------------------------------------------

    /// Blocking write.  Delegates to Policy / Wait for synchronisation.
    static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                       uint32_t timeout_ms);

    /// Blocking read.
    static Error read(uint16_t addr, uint8_t* buf, std::size_t len,
                      uint32_t timeout_ms);

    /// Repeated-START write-then-read.
    static Error write_read(uint16_t addr,
                            const uint8_t* tx_buf, std::size_t tx_len,
                            uint8_t*       rx_buf, std::size_t rx_len,
                            uint32_t       timeout_ms);
};

}  // namespace ll
}  // namespace stm32h7
}  // namespace backend
}  // namespace i2c
