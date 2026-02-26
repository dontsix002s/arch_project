/**
 * @file  board/rev_a/src/board_system_timer_stm32h7_ll.cpp
 * @brief TIM7 1 kHz system timebase for STM32H7 (LL / CMSIS implementation).
 *
 * Implements SystemTimerTraits::hw_init() for revision A.
 *
 * This file is the ONLY place in the BSP that includes vendor (CMSIS / LL)
 * headers.  All other BSP and driver headers remain vendor-free.
 *
 * Hardware configuration
 * ----------------------
 * - Timer   : TIM7 (basic timer, APB1 domain)
 * - APB1 timer clock: assumed to be 200 MHz after PLL configuration.
 *   Prescaler = 200 000 - 1 → 1 kHz tick (adjust if APB1 differs).
 * - Auto-reload = 1 (fires every 1 ms).
 * - NVIC priority is taken from SystemTimerTraits::irq_priority.
 *
 * Compiling
 * ---------
 * Requires vendor headers on the include path:
 *   external/cmsis/inc   – stm32h7xx.h, core_cm7.h
 *   external/ll/inc      – stm32h7xx_ll_bus.h, stm32h7xx_ll_tim.h
 * Enable the CMake target board_rev_a_impl (guarded by
 * BOARD_VENDOR_HEADERS_AVAILABLE) to compile this file.
 */

// Vendor headers are allowed only in .cpp files.
#include "stm32h7xx.h"          // CMSIS device header (defines TIM7, NVIC_*, etc.)
#include "stm32h7xx_ll_bus.h"   // LL_APB1_GRP1_EnableClock / TIM7
#include "stm32h7xx_ll_tim.h"   // LL_TIM_* API

#include "board/rev_a/inc/board_system_timer_traits.h"
#include "system/inc/system_timer.h"

namespace board {
namespace rev_a {

/// APB1 timer clock frequency assumed for this revision (Hz).
/// TIM7 is clocked from the APB1 timer clock.  Update this constant whenever
/// the clock tree is reconfigured; the prescaler below is derived from it.
static constexpr uint32_t kApb1TimerClkHz = 200'000'000U;  // 200 MHz

void SystemTimerTraits::hw_init() {
    // 1. Enable TIM7 peripheral clock.
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

    // 2. Configure TIM7 for a 1 kHz tick.
    //    Prescaler divides clock by (PSC + 1); ARR = period in ticks - 1.
    static_assert(kApb1TimerClkHz % SystemTimerTraits::tick_hz == 0U,
                  "APB1 timer clock is not an integer multiple of tick_hz");
    LL_TIM_SetPrescaler(TIM7, (kApb1TimerClkHz / SystemTimerTraits::tick_hz) - 1U);
    LL_TIM_SetAutoReload(TIM7, 1U - 1U);        // fire every 1 count = 1 ms
    LL_TIM_EnableARRPreload(TIM7);

    // 3. Clear any pending update flag before enabling the interrupt.
    LL_TIM_ClearFlag_UPDATE(TIM7);
    LL_TIM_EnableIT_UPDATE(TIM7);

    // 4. Configure and enable the TIM7 NVIC interrupt.
    NVIC_SetPriority(TIM7_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                         SystemTimerTraits::irq_priority, 0U));
    NVIC_EnableIRQ(TIM7_IRQn);

    // 5. Start the timer.
    LL_TIM_EnableCounter(TIM7);
}

}  // namespace rev_a
}  // namespace board

// ---------------------------------------------------------------------------
// TIM7 IRQ handler
// ---------------------------------------------------------------------------

extern "C" void TIM7_IRQHandler(void) {
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7)) {
        LL_TIM_ClearFlag_UPDATE(TIM7);
        system::SystemTimer<board::rev_a::SystemTimerTraits>::on_tick_isr();
    }
}
