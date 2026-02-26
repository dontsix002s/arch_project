/**
 * @file  drivers/i2c/wait/inc/i2c_wait_timed_busy.h
 * @brief TimedBusyWait – clock-driven busy-wait strategy.
 *
 * Unlike BaremetalWait (which uses a loop-count heuristic), TimedBusyWait
 * reads real wall-clock milliseconds from a Clock type supplied as a template
 * parameter.  This makes the timeout accurate regardless of CPU frequency or
 * compiler optimisation level.
 *
 * Clock contract
 * --------------
 * The Clock type must provide:
 *   static uint32_t now_ms();   // returns current time in milliseconds
 *
 * Wrap-safe subtraction
 * ---------------------
 * The elapsed-time calculation uses unsigned subtraction, which is safe across
 * a single uint32_t roll-over (~49 days at 1 kHz tick rate).
 *
 * Usage
 * -----
 * @code
 * #include "drivers/i2c/wait/inc/i2c_wait_timed_busy.h"
 * #include "board/rev_a/inc/board_timebase.h"
 *
 * using Wait = i2c::wait::TimedBusyWait<board::rev_a::time::TimebaseClock>;
 * @endcode
 *
 * Extension point
 * ---------------
 * Swap the Clock template argument to use any millisecond tick source without
 * changing any other code.
 */
#pragma once

#include <cstdint>

namespace i2c {
namespace wait {

/// Clock-driven busy-wait strategy.
///
/// @tparam Clock  Type that provides `static uint32_t now_ms()`.
template <typename Clock>
struct TimedBusyWait {
    /// Spin until @p predicate() returns true or @p timeout_ms elapses.
    ///
    /// @tparam Predicate  Callable with signature `bool ()`.
    /// @param  predicate  Condition to wait for.
    /// @param  timeout_ms Maximum wait time in milliseconds.
    /// @return            true if predicate was satisfied, false on timeout.
    template <typename Predicate>
    static bool wait_for(Predicate predicate, uint32_t timeout_ms) {
        const uint32_t start = Clock::now_ms();
        while (true) {
            if (predicate()) {
                return true;
            }
            // Unsigned subtraction is wrap-safe across a single roll-over.
            if ((Clock::now_ms() - start) >= timeout_ms) {
                return false;
            }
        }
    }
};

}  // namespace wait
}  // namespace i2c
