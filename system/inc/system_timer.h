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
 *   - static void hw_refresh()           – any periodic maintenance (optional).
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

#include <cstdint>

namespace system {

/// System timer façade parameterised by a hardware Traits type.
///
/// @tparam Traits  Hardware-specific traits (tick_hz, hw_init, hw_refresh).
template <typename Traits>
class SystemTimer {
public:
    static constexpr uint32_t tick_hz = Traits::tick_hz;

    static void init() {
        tick_count_ = 0U;
        Traits::hw_init();
    }

    /// Optional maintenance hook (can be empty).
    static void refresh() {
        Traits::hw_refresh();
    }

    /// Called from the hardware timer ISR to increment the tick counter.
    static void on_tick_isr() {
        ++tick_count_;
    }

    /// Return the current tick count (monotonically increasing).
    static uint32_t ticks() {
        return tick_count_;
    }

private:
    static volatile uint32_t tick_count_;
};

template <typename Traits>
volatile uint32_t SystemTimer<Traits>::tick_count_{0U};

}  // namespace system
