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
 * using Timer = stmfw::system::SystemTimer<stmfw::board::rev_a::SystemTimerTraits>;
 * Timer::init();
 * uint32_t ms = Timer::ticks();
 * @endcode
 */
#pragma once

#include <cstdint>

namespace stmfw::system
{
    /// System timer façade parameterised by a hardware Traits type.
    ///
    /// @tparam Traits  Hardware-specific traits (tick_hz, hw_init, hw_refresh).
    /// 

    template <typename Traits>
    class SystemTimer
    {
    public:
        static constexpr uint32_t tick_hz = Traits::tick_hz;

        //---------------------------------------------------------------------
        //  init()
        //---------------------------------------------------------------------
        static void init()
        {
            m_tick_count = 0U;
            Traits::hw_init();
        }

        //---------------------------------------------------------------------
        //  refresh()
        //---------------------------------------------------------------------
        static void refresh()
        {
            Traits::hw_refresh();
        }

        //---------------------------------------------------------------------
        //  on_tick_isr()
        //---------------------------------------------------------------------
        static void on_tick_isr()
        {
            ++m_tick_count;
        }
    
        //---------------------------------------------------------------------
        //  ticks()
        //---------------------------------------------------------------------
        static uint32_t ticks()
        {
            return (m_tick_count);
        }

    private:
        static volatile uint32_t m_tick_count;
    };

    template <typename Traits>
    volatile uint32_t SystemTimer<Traits>::m_tick_count{0U};
}
