/**
 * @file  board/rev_a/inc/board_system_timer_traits.h
 * @brief SystemTimerTraits for revision A (STM32H743ZI – TIM7, 1 kHz tick).
 *
 * This file defines the Traits type consumed by system::SystemTimer<Traits>.
 * All hardware-specific constants and function declarations live here; the
 * implementation is in board/rev_a/src/board_system_timer_stm32h7_ll.cpp.
 *
 * Application / device code must NOT include this header directly.
 * Use board/inc/board.h instead.
 */
#pragma once

#include <cstdint>

#include "board/inc/board_irq_priorities.h"

namespace board::rev_a {

struct SystemTimerTraits {
    /// Tick rate (1 kHz => 1 ms per tick).
    static constexpr uint32_t tick_hz = 1000U;

    /// TIM7 input clock frequency (APB1 timer clock) for this build.
    /// NOTE: for now this is an assumption; update if the clock tree changes.
    static constexpr uint32_t apb1_timer_clk_hz = 200'000'000U;

    /// NVIC priority for TIM7.
    static constexpr uint32_t irq_preempt_prio = board::kIrqPrio_SystemTimebase;
    static constexpr uint32_t irq_sub_prio = 0U;

    static void hw_init();
    static void hw_refresh();
};

}  // namespace board::rev_a
