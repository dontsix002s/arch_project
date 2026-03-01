/**
 * @file  board/rev_a/src/board_clock_stm32h7_ll.cpp
 * @brief STM32H750 RCC/PLL clock configuration (LL/CMSIS implementation).
 *
 * Implements board::rev_a::clock::apply() for the STM32H750.
 *
 * This file is the ONLY place in the BSP that includes RCC/PLL vendor headers.
 * All public board headers remain vendor-free.
 *
 * Profile parameters (480 MHz Performance example):
 *   HSE = 25 MHz external crystal
 *   PLL1: M=5, N=192, P=2  => VCO=960 MHz, SYSCLK=480 MHz
 *   AHB  prescaler = 2 => HCLK  = 240 MHz
 *   APB1 prescaler = 2 => APB1  = 120 MHz, APB1_TIM = 240 MHz
 *   APB2 prescaler = 2 => APB2  = 120 MHz, APB2_TIM = 240 MHz
 *
 * Balanced profile:
 *   HSI = 64 MHz, no PLL
 *   All buses at 64 MHz.
 *
 * LowPower profile:
 *   HSI = 64 MHz, no PLL (currently equivalent to Balanced; differentiated
 *   in a future revision when peripheral gating is added).
 *   All buses at 64 MHz.
 */

// -- Includes ------------------------------------
// Vendor headers confined to this .cpp only
#include "stm32h7xx.h"           // CMSIS device header
#include "stm32h7xx_ll_rcc.h"    // LL_RCC_* API
#include "stm32h7xx_ll_pwr.h"    // LL_PWR_* API
#include "stm32h7xx_ll_utils.h"  // LL_mDelay (not used; kept for reference)
#include "stm32h7xx_ll_bus.h"    // LL_AHB4_GRP1_EnableClock

#include "board/rev_a/inc/board_clock.h"
#include "board/rev_a/inc/board_timebase.h"
#include "system/inc/system_clocks.h"

namespace board::rev_a::clock {

	//-------------------------------------------------------------------------
    // Profile parameter tables
    //-------------------------------------------------------------------------
	struct ProfileParams
	{
		uint32_t pllm;           /// PLL1 /M divider
        uint32_t plln;           /// PLL1 *N multiplier
		uint32_t pllp;           /// PLL1 /P divider (SYSCLK source)
		uint32_t pllq;
		uint32_t pllr;

		uint32_t ahb_prescaler;  /// AHB divider (applied to SYSCLK => HCLK)
		uint32_t apb1_prescaler; /// APB1 divider
		uint32_t apb2_prescaler; /// APB2 divider
		
		// Resulting frequencies published to stmfw::system::clocks snapshot
        uint32_t sysclk_hz;      /// SYSCLK (PLL1P output or HSI)
        uint32_t hclk_hz;        /// HCLK (after AHB prescaler)
		uint32_t apb1_hz;
		uint32_t apb1_timer_hz;
        uint32_t apb2_hz;
        uint32_t apb2_timer_hz;
        bool     use_hse;        /// true = HSE source, false = HSI.
    };

	// Performance: HSE 25 MHz → PLL1 → 480 MHz SYSCLK
	//   HCLK = 480 / 2 = 240 MHz
	//   APB1 = 240 / 2 = 120 MHz  → APB1_TIM = 240 MHz
	//   APB2 = 240 / 2 = 120 MHz  → APB2_TIM = 240 MHz
	//   APB3 = 240 / 2 = 120 MHz
	//   APB4 = 240 / 2 = 120 MHz
    static constexpr ProfileParams kPerformanceParams = {
		.pllm           = 5U,
		.plln           = 192U,
		.pllp           = 2U,
		.pllq           = 2U,
        .pllr           = 2U,
		.ahb_prescaler  = 2U,
		.apb1_prescaler = 2U,
		.apb2_prescaler = 2U,
		.sysclk_hz      = 480'000'000U,
		.hclk_hz        = 240'000'000U,
		.apb1_hz        = 120'000'000U,
		.apb1_timer_hz  = 240'000'000U,
		.apb2_hz        = 120'000'000U,
		.apb2_timer_hz  = 240'000'000U,
		.use_hse        = true,		
    };

	// Balanced / LowPower: HSI 64 MHz, no PLL
    static constexpr ProfileParams kHsiParams = {
    /* pllm           = */ 0U,
    /* plln           = */ 0U,
    /* pllp           = */ 0U,
    /* ahb_prescaler  = */ 1U,
    /* apb1_prescaler = */ 1U,
    /* apb2_prescaler = */ 1U,
    /* sysclk_hz      = */ 64'000'000U,
    /* hclk_hz        = */ 64'000'000U,
    /* apb1_hz        = */ 64'000'000U,
    /* apb1_timer_hz  = */ 64'000'000U,
    /* apb2_hz        = */ 64'000'000U,
    /* apb2_timer_hz  = */ 64'000'000U,
    /* use_hse        = */ false,
    };

	///------------------------------------------------------------------------
	///  now_ms()
	///------------------------------------------------------------------------
    static inline uint32_t now_ms()
    { return board::rev_a::time::now_ms(); }

	///------------------------------------------------------------------------
	///  wait_until()
	///------------------------------------------------------------------------
	template <typename CondFunc>
    static bool wait_until(CondFunc cond, uint32_t timeout_ms)
    {
        const uint32_t start = now_ms();
        while (!cond())
        {
			if ((now_ms() - start) >= timeout_ms)
            { return (false); }
        }
        return (true);
    }

    //---------------------------------------------------------------------------
// HSI path
// ---------------------------------------------------------------------------

static bool apply_hsi(const ProfileParams& p)
{
    // Switch to HSI if not already the system clock source.
    if (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
    {
        LL_RCC_HSI_Enable();
        if (!wait_until([] { return LL_RCC_HSI_IsReady() != 0U; }, 10U))
        {
            return false;
        }
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
        if (!wait_until(
                [] { return LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSI; },
                10U))
        {
            return false;
        }
    }

    // Apply AHB / APB prescalers.
    LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    (void)p;
    return true;
}


    ///------------------------------------------------------------------------
    ///
    ///------------------------------------------------------------------------
    static bool apply_performance(const ProfileParams& p)
    {
	    constexpr uint32_t kHSI_Timeout = 10U;
		constexpr uint32_t kHSE_Timeout = 100U;
		constexpr uint32_t kPLL_Timeout = 10U;
		constexpr uint32_t kPwrReg_Timeout = 10U;

		constexpr uint32_t kLatency_Timeout = 50U;
		
		
		
		constexpr uint32_t kSetSysClock_Timeout = 10U;

		/* 1) Включить тактирование PWR (APB4) */
		//LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_PWR); //????
		
		
		
	
	
        // 1) Switch temporarily to HSI to allow PLL reconfiguration
        LL_RCC_HSI_Enable();
		if (!wait_until( [] { return LL_RCC_HSI_IsReady() != 0U; }, kHSI_Timeout) )
        { return (false); }
		
		LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
		if (!wait_until( [] { return LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_HSI; }, kSetSysClock_Timeout) )
        { return (false); }
		
		// 2) Disable PLL1 before reconfiguring
        LL_RCC_PLL1_Disable();
		if (!wait_until( [] { return LL_RCC_PLL1_IsReady() == 0U; }, kPLL_Timeout) )
        { return (false); }

        // 3) Enable HSE
        LL_RCC_HSE_Enable();
		if (!wait_until( [] { return LL_RCC_HSE_IsReady() != 0U; }, kHSE_Timeout) )
        { return (false); }

        // 4) Configure voltage scaling for 480 MHz operation (VOS0)
        LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
        LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
		if (!wait_until( [] { return LL_PWR_IsActiveFlag_VOS() != 0U; }, kPwrReg_Timeout) )
        { return (false); }

		// 5) Set FLASH latency
		LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
		if (!wait_until([] { return LL_FLASH_GetLatency() == LL_FLASH_LATENCY_4; }, kLatency_Timeout))
		{ return (false); };
		
        // 6) Configure PLL1
		LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
		LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8); // Вход VCO: 5MHz
		LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE); // Выход VCO: 960MHz
		
        LL_RCC_PLL1_SetM(p.pllm);
        LL_RCC_PLL1_SetN(p.plln);
        LL_RCC_PLL1_SetP(p.pllp);
		LL_RCC_PLL1_SetQ(p.pllq);
		LL_RCC_PLL1_SetR(p.pllr);
		
        LL_RCC_PLL1P_Enable();
		LL_RCC_PLL1R_Enable();

        // 7) Enable PLL1 and wait for lock
        LL_RCC_PLL1_Enable();
		if (!wait_until([] { return LL_RCC_PLL1_IsReady() != 0U; }, kPLL_Timeout))
        { return (false); }

        // 8) Apply AHB / APB prescalers before switching SYSCLK source
		LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);		
        LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
        LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
        LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
		LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
		LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);

        // 9) Switch SYSCLK to PLL1
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
		if (!wait_until([] { return LL_RCC_GetSysClkSource() == LL_RCC_SYS_CLKSOURCE_STATUS_PLL1; }, kSetSysClock_Timeout) )
        { return (false); }
		
        return (true);
    }



    ///-----------------------------
    ///
    ///-------------
    bool apply(Profile profile)
	{
        const ProfileParams* p = nullptr;
        bool ok = false;

        switch (profile)
        {
            case Profile::Performance:
		        p = &kPerformanceParams;
                ok = apply_performance(*p);
                break;

            case Profile::Balanced:
            case Profile::LowPower:
                p = &kHsiParams;
                ok = apply_hsi(*p);
                break;
        }

        if (ok && p != nullptr)
        {
        stmfw::system::clocks::set({
            p->sysclk_hz,
            p->hclk_hz,
            p->apb1_hz,
            p->apb1_timer_hz,
            p->apb2_hz,
            p->apb2_timer_hz,
        });
        }

        return (ok);
    }
}
