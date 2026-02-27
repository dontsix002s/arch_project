/**
 * @file  board/rev_a/inc/board_system_timer_traits.h
 * @brief SystemTimerTraits for revision A (STM32H7 – TIM6 or TIM7, 1 kHz tick).
 *
 * This file defines the Traits type consumed by stmfw::system::SystemTimer<Traits>.
 * All hardware-specific constants and function declarations live here; the
 * implementation is in board/rev_a/src/board_system_timer_stm32h7_ll.cpp.
 *
 * Application / device code must NOT include this header directly.
 * Use board/inc/board.h instead.
 */
#pragma once

#include <cstdint>

#include "board/inc/board_irq_priorities.h"
#include "system/inc/system_clocks.h"

namespace board::rev_a {

struct SystemTimerTraits {
    /// Available hardware timers for the system timebase.
    enum class TimerId
    {
        Tim6,
        Tim7,
    };

    /// Selected hardware timer (default: TIM7).
    static constexpr TimerId timer_id = TimerId::Tim7;

    /// Tick rate (1 kHz => 1 ms per tick).
    static constexpr uint32_t tick_hz = 1000U;

    /// Return the current APB1 timer clock frequency from the system clock
    /// snapshot.  This allows hw_refresh() to recompute PSC/ARR correctly
    /// after a clock reconfiguration without hard-coded constants.
    static uint32_t timer_clk_hz()
    {
        return stmfw::system::clocks::get().apb1_timer_hz;
    }

    /// NVIC priority for the selected timer.
    static constexpr uint32_t irq_preempt_prio = board::kIrqPrio_SystemTimebase;
    static constexpr uint32_t irq_sub_prio = 0U;

    static void hw_init();
    static void hw_refresh();
};

}  // namespace board::rev_a
