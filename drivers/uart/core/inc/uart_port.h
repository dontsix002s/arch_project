#pragma once

// -- Includes ------------------------------------
#include <cstddef>
#include <cstdint>
#include "drivers/uart/core/inc/uart_error.h"

namespace uart {

	///------------------------------------------------------------------------
	///  Port
	///------------------------------------------------------------------------
	template <typename Backend, typename Policy>
	struct Port
	{
		///--------------------------------------------------------------------
		/// init()
		///--------------------------------------------------------------------
		static void init()
		{
			Backend::init();
		}

		///--------------------------------------------------------------------
		///  deinit()
		///--------------------------------------------------------------------
		static void deinit()
		{
			Backend::deinit();
		}

		///--------------------------------------------------------------------
		///  write()
		///--------------------------------------------------------------------
		static Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return Backend::write(buf, len, timeout_ms);
		}

		///--------------------------------------------------------------------
		///  read()
		///--------------------------------------------------------------------
		static Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return Backend::read(buf, len, timeout_ms);
		}

		// Board-level glue calls this (stable entry point)
		static void on_irq_isr() { Backend::on_irq_isr(); }
	};
}