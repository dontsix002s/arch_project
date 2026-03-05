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

#include "board/rev_a/inc/pin_map.h"
#include "mcu/stm32h7/inc/mcu_map.h"

 // UART (B2 split: MCU periph + board cfg)
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_mcu_periph.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_board_cfg.h"
#include "drivers/uart/backend/stm32h7/low_layer/inc/uart_ll_backend.h"
#include "drivers/uart/core/inc/uart_port.h"
#include "drivers/uart/policy/inc/uart_policy_polling.h"

///----------------------------------------------------------------------------
/// I2C1 per-peripheral namespace
///----------------------------------------------------------------------------
namespace board::rev_a::i2c1 {

	inline constexpr i2c::backend::stm32h7::PeriphDescriptor PeriphDesc = {
		.base_address = 0x40005400U,
		// I2C1 base on STM32H7
		.instance_index = 0U,
		.clock_hz = 100'000'000U,
		// APB1 clock (placeholder value)
	};

	using Policy = i2c::policy::PollingPolicy;
	using Wait = i2c::wait::TimedBusyWait<board::rev_a::time::TimebaseClock>;
	using Backend = i2c::backend::stm32h7::ll::LlBackend<PeriphDesc, Policy, Wait>;
	using Bus = i2c::Bus<Backend, Policy>;

	inline void on_event_isr() { Bus::on_event_isr(); }
	inline void on_error_isr() { Bus::on_error_isr(); }
}

///----------------------------------------------------------------------------
/// UART1 per-peripheral namespace (B2)
///----------------------------------------------------------------------------
namespace board::rev_a::uart1 {

	// MCU static data (does not depend on pins/board)
	inline constexpr uart::backend::stm32h7::McuPeriph Mcu = {
		.usart_base = mcu::stm32h750::kUsart1Base,
		.instance_index = 0U,
	};

	// Board config (pins + baud + clock)
	inline constexpr uart::backend::stm32h7::BoardCfg Cfg = {
		.clock_hz = 100'000'000U,
		// placeholder APB2
		.baud = 115200U,
		.tx = {
		board::rev_a::pins::kUart1TxPort,
		board::rev_a::pins::kUart1TxPin,
		board::rev_a::pins::kUart1TxAf,
	},
		.rx = {
		board::rev_a::pins::kUart1RxPort,
		board::rev_a::pins::kUart1RxPin,
		board::rev_a::pins::kUart1RxAf,
	},
	};

	using Policy = uart::policy::PollingPolicy;
	using Backend = uart::backend::stm32h7::ll::LlBackend<Mcu, Policy>;
	using RawPort = uart::Port<Backend, Policy>;

	// Wrapper Port so callers can keep using board::...::uart1::Port::init()
	// and we guarantee bind() happens first.
	struct Port
	{
		static void init()
		{
			Backend::bind(Cfg);
			RawPort::init(); // calls Backend::init()
		}

		static void deinit() { RawPort::deinit(); }

		static uart::Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return RawPort::write(buf, len, timeout_ms);
		}

		static uart::Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return RawPort::read(buf, len, timeout_ms);
		}

		static void on_irq_isr() { RawPort::on_irq_isr(); }
	};

	// Stable IRQ entry point (called from board-level glue)
	inline void on_irq_isr() { Port::on_irq_isr(); }
}