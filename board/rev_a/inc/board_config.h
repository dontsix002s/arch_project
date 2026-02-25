/**
 * @file  board/rev_a/inc/board_config.h
 * @brief Board-level I2C configuration for revision A of the reference board.
 *
 * This file is the single place that:
 *   1. Declares the PeriphDescriptor constants for each on-board I2C bus.
 *   2. Selects the backend type (LL / HAL / CMSIS) and the policy/wait pair.
 *   3. Exposes a ready-made Bus alias for each peripheral.
 *
 * Application code and device drivers should include this header (or a thin
 * wrapper) rather than including backend or policy headers directly.
 *
 * Changing the backend
 * --------------------
 * To switch from the LL backend to the HAL backend for I2C1:
 *   1. Replace the LlBackend alias below with HalBackend.
 *   2. Adjust the Policy/Wait types if desired.
 *   3. Recompile; no other file needs to change.
 */
#pragma once

#include "drivers/i2c/backend/stm32h7/common/inc/i2c_stm32h7_periph.h"
#include "drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h"
#include "drivers/i2c/core/inc/i2c_bus.h"
#include "drivers/i2c/policy/inc/i2c_policy_polling.h"
#include "drivers/i2c/wait/inc/i2c_wait_baremetal.h"

namespace board {
namespace rev_a {

// ---------------------------------------------------------------------------
// I2C1 peripheral descriptor
// ---------------------------------------------------------------------------

/// Static descriptor for the I2C1 peripheral on the STM32H743ZI Nucleo board.
///
/// Handler slots are left nullptr here.  The IRQ glue file
///   drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp
/// populates them inside LlBackend::init() when an interrupt-driven policy is
/// selected.
inline constexpr i2c::backend::stm32h7::PeriphDescriptor kI2c1Periph = {
    /* base_address   = */ 0x40005400U,   // I2C1 base on STM32H7
    /* instance_index = */ 0U,
    /* clock_hz       = */ 100'000'000U,  // APB1 clock (placeholder value)
    /* IrqEventHandler= */ nullptr,
    /* IrqErrorHandler= */ nullptr,
    /* DmaTxHandler   = */ nullptr,
    /* DmaRxHandler   = */ nullptr,
};

// ---------------------------------------------------------------------------
// Ready-made Bus aliases
// ---------------------------------------------------------------------------

/// I2C1 on revision-A board, driven by the LL backend in polling mode.
///
/// For interrupt-driven mode replace PollingPolicy with IrqPolicy and
/// BaremetalWait with ThreadXWait (or FreeRTOSWait, etc.).
using I2c1Bus = i2c::Bus<
    i2c::backend::stm32h7::ll::LlBackend<
        kI2c1Periph,
        i2c::policy::PollingPolicy,
        i2c::wait::BaremetalWait>,
    i2c::policy::PollingPolicy>;

}  // namespace rev_a
}  // namespace board
