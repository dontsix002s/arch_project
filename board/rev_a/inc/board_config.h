/**
 * @file  board/rev_a/inc/board_config.h
 * @brief Board-level peripheral configuration for revision A of the reference board.
 *
 * This file is the single place that:
 *   1. Declares the PeriphDescriptor constants for each on-board I2C bus.
 *   2. Selects the backend type (LL / HAL / CMSIS) and the policy/wait pair.
 *   3. Exposes ready-made type aliases for each peripheral inside per-peripheral
 *      namespaces (e.g. board::rev_a::i2c1).
 *
 * Application code and device drivers must include board/inc/board.h rather
 * than this header directly.  board.h re-exports the active revision via the
 * board::i2c1 namespace alias so that code never needs to name a revision.
 *
 * Changing the backend
 * --------------------
 * To switch from the LL backend to the HAL backend for I2C1:
 *   1. Replace the LlBackend alias below with HalBackend.
 *   2. Adjust the Policy/Wait types if desired.
 *   3. Recompile; no other file needs to change.
 */
#pragma once

#include "drivers/i2c/backend/stm32h7/common/inc/i2c_stm32h7_periph.h"
#include "drivers/i2c/backend/stm32h7/low_layer/inc/i2c_ll_backend.h"
#include "drivers/i2c/core/inc/i2c_bus.h"
#include "drivers/i2c/policy/inc/i2c_policy_polling.h"
#include "drivers/i2c/wait/inc/i2c_wait_timed_busy.h"
#include "board/rev_a/inc/board_timebase.h"
//
namespace board::rev_a {
//
//// ---------------------------------------------------------------------------
//// I2C1 per-peripheral namespace
//// ---------------------------------------------------------------------------
//
	namespace i2c1 {

/// Static descriptor for the I2C1 peripheral on the STM32H743ZI Nucleo board.
/////
///// Handler slots are left nullptr here.  The IRQ glue file
/////   drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp
///// populates them inside LlBackend::init() when an interrupt-driven policy is
///// selected.
		inline constexpr i2c::backend::stm32h7::PeriphDescriptor PeriphDesc = {
				/* base_address   = */ 0x40005400U,   // I2C1 base on STM32H743ZI
    /* instance_index = */ 0U,
    /* clock_hz       = */ 100'000'000U,  // APB1 clock (placeholder value)
    /* IrqEventHandler= */ nullptr,
    /* IrqErrorHandler= */ nullptr,
    /* DmaTxHandler   = */ nullptr,
    /* DmaRxHandler   = */ nullptr,
        };
		
		
		/// Transfer mode policy: polling (no interrupts or DMA)
        using Policy = i2c::policy::PollingPolicy;
		
		/// Wait strategy: real-time busy-wait backed by the board timebase clock
		using Wait = i2c::wait::TimedBusyWait<board::rev_a::time::TimebaseClock>;

		/// Concrete LL backend for I2C1
        using Backend = i2c::backend::stm32h7::ll::LlBackend<PeriphDesc, Policy, Wait>;

        /// Ready-made I2C bus alias for application / device-driver use
        using Bus = i2c::Bus<Backend, Policy>;
			
		inline void on_event_isr() { Bus::on_event_isr(); }
		inline void on_error_isr() { Bus::on_error_isr(); }

    }  // namespace i2c1
	
	
	//-------------------------------
	//
	//------------------------
	namespace uart1 {
		
		
		// using Port = uart::Port<Backend>; (или другое)
		//inline void on_irq_isr() { Port::on_irq_isr(); }  // или Backend::on_irq_isr()
		
		
	}
	

		
		
}
