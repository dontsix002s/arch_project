/**
 * @file  drivers/i2c/wait/inc/i2c_wait_baremetal.h
 * @brief BaremetalWait – bare-metal (spinning) wait strategy.
 *
 * The wait_for() template method spins in a tight loop, repeatedly evaluating
 * the supplied predicate until it returns true or the deadline elapses.
 *
 * Timer source
 * ------------
 * The current implementation uses a simple decrement counter as a placeholder.
 * Replace the body of wait_for() with a SysTick- or DWT-based real-time check
 * once the board BSP provides a tick source.
 *
 * Extension point
 * ---------------
 * For RTOS environments replace BaremetalWait with ThreadXWait (or a custom
 * wait type) in the Bus instantiation.  The wait strategy is a template
 * parameter so the swap requires no changes to backend logic.
 */
#pragma once

#include <cstdint>

namespace i2c {
namespace wait {

/// Bare-metal busy-wait strategy.
struct BaremetalWait {
    /// Spin until @p predicate() returns true or @p timeout_ms elapses.
    ///
    /// @tparam Predicate  Callable with signature `bool ()`.
    /// @param  predicate  Condition to wait for.
    /// @param  timeout_ms Maximum wait time in milliseconds.
    /// @return            true if predicate was satisfied, false on timeout.
    ///
    /// @note The current loop counter is a placeholder; replace with a real
    ///       hardware tick source (SysTick / DWT cycle counter).
    template <typename Predicate>
    static bool wait_for(Predicate predicate, uint32_t timeout_ms) {
        // Placeholder: scale factor assumes roughly 1 000 loop iterations / ms.
        // TODO: replace with a hardware-timer-based deadline.
        uint32_t iterations = timeout_ms * 1000U;
        while (iterations--) {
            if (predicate()) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace wait
}  // namespace i2c
