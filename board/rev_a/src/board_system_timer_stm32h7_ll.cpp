/**
 * @file  board/rev_a/src/board_system_timer_stm32h7_ll.cpp
 * @brief TIM6/TIM7 1 kHz system timebase for STM32H7 (LL / CMSIS implementation).
 *
 * Implements SystemTimerTraits::hw_init() and hw_refresh() for revision A.
 * The active timer is selected by SystemTimerTraits::timer_id.
 *
 * This file is the ONLY place in the BSP that includes vendor (CMSIS / LL)
 * headers.  All other BSP and driver headers remain vendor-free.
 */

// Vendor headers are allowed only in .cpp files.
#include "stm32h7xx.h"          // CMSIS device header (defines TIM6/TIM7, NVIC_*, etc.)
#include "stm32h7xx_ll_bus.h"   // LL_APB1_GRP1_EnableClock
#include "stm32h7xx_ll_tim.h"   // LL_TIM_* API

#include "board/rev_a/inc/board_system_timer_traits.h"
#include "system/inc/system_timer.h"

namespace board::rev_a {

// ---------------------------------------------------------------------------
// Timer descriptor resolved at runtime from timer_id.
// ---------------------------------------------------------------------------

struct TimerDesc
{
    TIM_TypeDef* tim;
    IRQn_Type    irqn;
    uint32_t     rcc_periph;  ///< LL_APB1_GRP1_PERIPH_TIMx constant.
};

static TimerDesc resolve_timer(SystemTimerTraits::TimerId id)
{
    switch (id)
    {
        case SystemTimerTraits::TimerId::Tim6:
            return { TIM6, TIM6_DAC_IRQn, LL_APB1_GRP1_PERIPH_TIM6 };
        case SystemTimerTraits::TimerId::Tim7:
        default:
            return { TIM7, TIM7_IRQn, LL_APB1_GRP1_PERIPH_TIM7 };
    }
}

// ---------------------------------------------------------------------------
// Shared PSC/ARR computation
// ---------------------------------------------------------------------------

static void configure_timer_registers(TIM_TypeDef* tim, uint32_t clk_hz)
{
    static constexpr uint32_t kBaseHz = 1'000'000U;

    // A clock below 1 MHz would produce a zero prescaler (underflow) which
    // indicates a misconfiguration; assert in debug builds.
    static_assert(SystemTimerTraits::tick_hz <= kBaseHz,
                  "tick_hz must not exceed 1 MHz (kBaseHz)");
    // clk_hz must be >= kBaseHz to produce a valid PSC >= 0.
    // This is a runtime check; assert if violated.
    if (clk_hz < kBaseHz)
    {
        // Invalid clock; do not reconfigure the timer to avoid corruption.
        return;
    }

    const uint32_t psc = (clk_hz / kBaseHz) - 1U;
    const uint32_t arr = (kBaseHz / SystemTimerTraits::tick_hz) - 1U;  // 999 for 1 kHz

    LL_TIM_SetPrescaler(tim, psc);
    LL_TIM_SetAutoReload(tim, arr);
    LL_TIM_EnableARRPreload(tim);

    // Force update event so PSC/ARR are taken immediately.
    LL_TIM_GenerateEvent_UPDATE(tim);

    // Reset counter to ensure first period is exact.
    LL_TIM_SetCounter(tim, 0U);
}

// ---------------------------------------------------------------------------
//  SystemTimerTraits::hw_init()
// ---------------------------------------------------------------------------
void SystemTimerTraits::hw_init()
{
    const TimerDesc td = resolve_timer(timer_id);

    // Enable peripheral clock.
    LL_APB1_GRP1_EnableClock(td.rcc_periph);

    // Stop timer while configuring (deterministic).
    LL_TIM_DisableCounter(td.tim);
    LL_TIM_DisableIT_UPDATE(td.tim);

    // Configure PSC/ARR using current clock snapshot.
    configure_timer_registers(td.tim, timer_clk_hz());

    // Clear any pending update flag before enabling the interrupt.
    LL_TIM_ClearFlag_UPDATE(td.tim);
    LL_TIM_EnableIT_UPDATE(td.tim);

    // Configure and enable NVIC interrupt.
    NVIC_SetPriority(td.irqn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                                         irq_preempt_prio, irq_sub_prio));
    NVIC_EnableIRQ(td.irqn);

    // Start the timer.
    LL_TIM_EnableCounter(td.tim);
}

// ---------------------------------------------------------------------------
//  SystemTimerTraits::hw_refresh()
//  Recomputes PSC/ARR using the current timer_clk_hz() snapshot so that the
//  1 kHz tick rate is maintained after a clock tree reconfiguration.
// ---------------------------------------------------------------------------
void SystemTimerTraits::hw_refresh()
{
    const TimerDesc td = resolve_timer(timer_id);

    // Reconfigure registers with updated clock.
    configure_timer_registers(td.tim, timer_clk_hz());
}

}  // namespace board::rev_a

//-----------------------------------------------------------------------------
// TIM7 IRQ handler
// Only processes the update event when TIM7 is the selected system timebase.
//-----------------------------------------------------------------------------
extern "C" void TIM7_IRQHandler(void)
{
    if (board::rev_a::SystemTimerTraits::timer_id != board::rev_a::SystemTimerTraits::TimerId::Tim7)
    {
        return;
    }
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7))
    {
        LL_TIM_ClearFlag_UPDATE(TIM7);
        system::SystemTimer<board::rev_a::SystemTimerTraits>::on_tick_isr();
    }
}

//-----------------------------------------------------------------------------
// TIM6 IRQ handler (shared with DAC on STM32H7)
// Only processes the update event when TIM6 is the selected system timebase.
//-----------------------------------------------------------------------------
extern "C" void TIM6_DAC_IRQHandler(void)
{
    if (board::rev_a::SystemTimerTraits::timer_id != board::rev_a::SystemTimerTraits::TimerId::Tim6)
    {
        return;
    }
    if (LL_TIM_IsActiveFlag_UPDATE(TIM6))
    {
        LL_TIM_ClearFlag_UPDATE(TIM6);
        system::SystemTimer<board::rev_a::SystemTimerTraits>::on_tick_isr();
    }
}
