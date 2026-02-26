/**
 * @file  board/rev_a/src/board_system_timer_stm32h7_ll.cpp
 * @brief TIM7 1 kHz system timebase for STM32H7 (LL / CMSIS implementation).
 *
 * Implements SystemTimerTraits::hw_init() for revision A.
 *
 * This file is the ONLY place in the BSP that includes vendor (CMSIS / LL)
 * headers.  All other BSP and driver headers remain vendor-free.
 */

// Vendor headers are allowed only in .cpp files.
#include "stm32h7xx.h"          // CMSIS device header (defines TIM7, NVIC_*, etc.)
#include "stm32h7xx_ll_bus.h"   // LL_APB1_GRP1_EnableClock / TIM7
#include "stm32h7xx_ll_tim.h"   // LL_TIM_* API

#include "board/rev_a/inc/board_system_timer_traits.h"
#include "system/inc/system_timer.h"

namespace board::rev_a {

    //-------------------------------------------------------------------------
    //  SystemTimerTraits::hw_init()
    //-------------------------------------------------------------------------
    void SystemTimerTraits::hw_init()
    {    
        // Enable TIM7 peripheral clock.
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

        // Stop timer while configuring (deterministic).
        LL_TIM_DisableCounter(TIM7);
        LL_TIM_DisableIT_UPDATE(TIM7);
    
        // Configure TIM7 for 1 ms update events:
        //  - Prescale TIM7 clock down to 1 MHz (1 us per timer tick)
        //  - ARR so that update frequency is tick_hz (e.g. 1000 => 1 ms)
        static constexpr uint32_t kBaseHz = 1'000'000U;

        static_assert(apb1_timer_clk_hz % kBaseHz == 0U,
                      "apb1_timer_clk_hz must be an integer multiple of 1 MHz");
        static_assert(kBaseHz % tick_hz == 0U,
                      "1 MHz base must be an integer multiple of tick_hz");

        const uint32_t psc = (apb1_timer_clk_hz / kBaseHz) - 1U;
        const uint32_t arr = (kBaseHz / tick_hz) - 1U;  // for 1 kHz => 999

    
        LL_TIM_SetPrescaler(TIM7, psc);
        LL_TIM_SetAutoReload(TIM7, arr);
        LL_TIM_EnableARRPreload(TIM7);

        // Force update event so PSC/ARR are taken immediately.
        LL_TIM_GenerateEvent_UPDATE(TIM7);

        // Reset counter to ensure first period is exact.
        LL_TIM_SetCounter(TIM7, 0U);

        // Clear any pending update flag before enabling the interrupt.
        LL_TIM_ClearFlag_UPDATE(TIM7);
        LL_TIM_EnableIT_UPDATE(TIM7);

        // Configure and enable TIM7 NVIC interrupt.
        NVIC_SetPriority(TIM7_IRQn,
                         NVIC_EncodePriority(NVIC_GetPriorityGrouping(), irq_preempt_prio, irq_sub_prio));
        NVIC_EnableIRQ(TIM7_IRQn);

        // Start the timer.
        LL_TIM_EnableCounter(TIM7);
    }

    //-------------------------------------------------------------------------
    void SystemTimerTraits::hw_refresh()
    {
    // No periodic maintenance required for basic TIM7 tick.
    }

}  // namespace board::rev_a

//-----------------------------------------------------------------------------
// TIM7 IRQ handler
//-----------------------------------------------------------------------------
extern "C" void TIM7_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7))
    {
        LL_TIM_ClearFlag_UPDATE(TIM7);
        system::SystemTimer<board::rev_a::SystemTimerTraits>::on_tick_isr();
    }
}
