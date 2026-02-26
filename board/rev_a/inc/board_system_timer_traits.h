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

namespace board {
namespace rev_a {

/// Traits type for the revision-A system timer (TIM7, 1 kHz).
struct SystemTimerTraits {
    /// Tick frequency in Hz (1 tick = 1 ms).
    static constexpr uint32_t tick_hz = 1000U;

    /// NVIC preemption priority for the TIM7 global interrupt.
    static constexpr uint32_t irq_priority = board::kIrqPrio_SystemTimebase;

    /// Configure TIM7 for a 1 kHz tick and enable its NVIC interrupt.
    /// Implemented in board_system_timer_stm32h7_ll.cpp.
    static void hw_init();

    /// Periodic maintenance hook (no-op for TIM7 – flag cleared in ISR).
    static void hw_refresh() {}
};

}  // namespace rev_a
}  // namespace board
