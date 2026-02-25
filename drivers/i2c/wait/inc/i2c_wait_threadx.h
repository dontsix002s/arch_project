/**
 * @file  drivers/i2c/wait/inc/i2c_wait_threadx.h
 * @brief ThreadXWait – Azure RTOS ThreadX blocking wait strategy.
 *
 * Instead of spinning, the caller is suspended on a counting semaphore.  The
 * ISR (or DMA TC handler) calls the backend's on_transfer_complete() method
 * which posts the semaphore, unblocking the waiting thread.
 *
 * Integration steps
 * -----------------
 * 1. Ensure tx_api.h is on the include path (ThreadX public header).
 * 2. Provide a TX_SEMAPHORE instance in the backend's per-peripheral state.
 * 3. Call tx_semaphore_create() during Backend::init().
 * 4. In the ISR glue function, call tx_semaphore_put() to signal completion.
 *
 * Extension point
 * ---------------
 * Other RTOS wait strategies (FreeRTOS, Zephyr) follow the same pattern:
 * implement wait_for() using the RTOS blocking primitive (binary semaphore,
 * event flags, etc.) and swap the Wait template parameter in the Bus
 * instantiation.
 */
#pragma once

#include <cstdint>

namespace i2c {
namespace wait {

/// ThreadX RTOS blocking wait strategy.
///
/// @note  tx_api.h is NOT included here to keep the header free of vendor
///        dependencies.  The backend implementation file that calls wait_for()
///        must include tx_api.h before including this header.
struct ThreadXWait {
    /// Block the calling thread until @p predicate() returns true or
    /// @p timeout_ms elapses.
    ///
    /// @tparam Predicate  Callable with signature `bool ()`.
    /// @param  predicate  Condition to wait for (checked after semaphore post).
    /// @param  timeout_ms Maximum wait time in milliseconds.
    /// @return            true if predicate was satisfied, false on timeout.
    ///
    /// @note  Replace the stub body below with a real ThreadX semaphore wait:
    ///        tx_semaphore_get(&sem, ms_to_ticks(timeout_ms))
    template <typename Predicate>
    static bool wait_for(Predicate predicate, uint32_t timeout_ms) {
        // TODO: replace with tx_semaphore_get() on the backend's semaphore.
        (void)timeout_ms;
        return predicate();
    }
};

}  // namespace wait
}  // namespace i2c
