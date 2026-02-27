/**
 * @file  board/inc/board_irq_priorities.h
 * @brief Board-wide NVIC interrupt priority constants.
 *
 * All IRQ priorities used by the BSP are collected here so that priority
 * assignments remain consistent across peripherals.
 *
 * Priority scheme (lower numeric value = higher priority):
 *   0  – reserved for the highest-priority fault handlers (HardFault, etc.)
 *   1  – critical real-time tasks (not used by BSP peripherals)
 *   2  – system timebase (TIM7 tick)
 *   3+ – peripheral drivers
 */
#pragma once

#include <cstdint>

namespace stmfw::board {

/// NVIC preemption priority for the system timebase timer interrupt (TIM7).
inline constexpr uint32_t kIrqPrio_SystemTimebase = 2U;

}  // namespace stmfw::board
