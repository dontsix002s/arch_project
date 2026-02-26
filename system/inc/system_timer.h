/**
 * @file  system/inc/system_timer.h
 * @brief Portable system timer abstraction.
 *
 * Provides a template class parameterised over a Traits type that supplies
 * hardware-specific initialisation and refresh hooks.
 *
 * Traits contract
 * ---------------
 * The Traits type must provide:
 *   - static constexpr uint32_t tick_hz  – tick frequency in Hz (e.g. 1000).
 *   - static void hw_init()              – configure and start the hardware timer.
 *   - static void hw_refresh()           – any periodic maintenance (e.g. clear flag).
 *
 * Usage
 * -----
 * @code
 * #include "system/inc/system_timer.h"
 * #include "board/rev_a/inc/board_system_timer_traits.h"
 *
 * using Timer = system::SystemTimer<board::rev_a::SystemTimerTraits>;
 * Timer::init();
 * uint32_t ms = Timer::ticks();
 * @endcode
 */
#pragma once

#include <atomic>
#include <cstdint>

namespace system {

/// System timer façade parameterised by a hardware Traits type.
///
/// @tparam Traits  Hardware-specific traits (tick_hz, hw_init, hw_refresh).
template <typename Traits>
class SystemTimer {
public:
    /// Tick frequency provided by Traits (e.g. 1000 for 1 kHz / 1 ms ticks).
    static constexpr uint32_t tick_hz = Traits::tick_hz;

    /// Initialise the underlying hardware timer via Traits::hw_init().
    static void init() {
        tick_count_.store(0U, std::memory_order_relaxed);
        Traits::hw_init();
    }

    /// Perform any periodic maintenance (call from a periodic ISR or task if needed).
    static void refresh() {
        Traits::hw_refresh();
    }

    /// Called from the hardware timer ISR to increment the tick counter.
    static void on_tick_isr() {
        tick_count_.fetch_add(1U, std::memory_order_relaxed);
    }

    /// Return the current tick count (monotonically increasing).
    static uint32_t ticks() {
        return tick_count_.load(std::memory_order_relaxed);
    }

private:
    // std::atomic ensures compiler-visible ordering between ISR and main context.
    // memory_order_relaxed is sufficient for a single monotonic counter on a
    // single-core Cortex-M device where no data dependency exists beyond the
    // counter value itself.
    static std::atomic<uint32_t> tick_count_;
};

template <typename Traits>
std::atomic<uint32_t> SystemTimer<Traits>::tick_count_{0U};

}  // namespace system
