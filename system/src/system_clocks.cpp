/**
 * @file  system/src/system_clocks.cpp
 * @brief System clock snapshot implementation.
 *
 * Implements stmfw::system::clocks::reset_defaults(), get(), and set().
 * The snapshot is stored in a single static variable and updated by the BSP
 * after each RCC/PLL reconfiguration.
 */

// -- Includes ------------------------------------
#include "system/inc/system_clocks.h"


namespace stmfw::system::clocks {

    namespace {
		
	    ///--------------------------------------------------------------
        /// Reset-default clock snapshot (64 MHz HSI, no prescalers)
	    ///--------------------------------------------------------------
		static constexpr Clocks kResetDefaults = {
			.sysclk_hz     = 64'000'000U,
			.hclk_hz       = 64'000'000U,
			.apb1_hz       = 64'000'000U,
			.apb1_timer_hz = 64'000'000U,
			.apb2_hz       = 64'000'000U,
			.apb2_timer_hz = 64'000'000U,
			.apb3_hz       = 64'000'000U,
			.apb4_hz       = 64'000'000U,
        };

        /// Internal stored snapshot; initialised to reset defaults
        static Clocks s_clocks = kResetDefaults;
    }

	//-------------------------------------------------------------------------
	//  reset_defaults()
	//-------------------------------------------------------------------------
    Clocks reset_defaults()
    { return (kResetDefaults); }

	//-------------------------------------------------------------------------
	//  get()
	//-------------------------------------------------------------------------
    Clocks get()
    { return (s_clocks); }

	//-------------------------------------------------------------------------
	//  set()
	//-------------------------------------------------------------------------
    void set(const Clocks& clocks)
    { s_clocks = clocks; }
}
