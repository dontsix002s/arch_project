#pragma once
#include <cstddef>
#include <cstdint>

#include "drivers/uart/core/inc/uart_error.h"

namespace uart {

	template <typename Backend, typename Policy>
	struct Port
	{
		static void init() { Backend::init(); }
		static void deinit() { Backend::deinit(); }

		static Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return Backend::write(buf, len, timeout_ms);
		}

		static Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms = 100U)
		{
			return Backend::read(buf, len, timeout_ms);
		}

		// Board-level glue calls this (stable entry point)
		static void on_irq_isr() { Backend::on_irq_isr(); }
	};

}  // namespace uart