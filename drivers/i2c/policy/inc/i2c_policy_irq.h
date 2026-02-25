/**
 * @file  drivers/i2c/policy/inc/i2c_policy_irq.h
 * @brief IrqPolicy – interrupt-driven transfer mode.
 *
 * The backend initiates the transfer and then suspends the caller (via the
 * Wait strategy).  An ISR fires on each byte transferred and on
 * completion/error, posting a semaphore or flag that the Wait strategy polls.
 *
 * Trade-offs
 * ----------
 * + CPU is not blocked during byte-to-byte gaps.
 * + Suitable for moderate-rate buses and RTOS tasks.
 * - Requires ISR glue wiring (see i2c_irq_glue.cpp placeholder).
 * - Higher complexity than polling.
 *
 * IRQ glue
 * --------
 * The companion file
 *   drivers/i2c/backend/stm32h7/<type>/src/i2c_irq_glue.cpp
 * must define the STM32 IRQ handler functions and populate the
 * IrqEventHandler / IrqErrorHandler slots in the PeriphDescriptor.
 */
#pragma once

namespace i2c {
namespace policy {

/// Tag type that selects interrupt-driven transfer mode.
struct IrqPolicy {
    struct type_tag {};
};

}  // namespace policy
}  // namespace i2c
