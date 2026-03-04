#pragma once
#include <cstddef>
#include <cstdint>

#include "drivers/uart/core/inc/uart_error.h"

namespace uart {

	struct Traits
	{
		// Lifecycle
		static void init();
		static void deinit();

		// Blocking-ish API with timeout_ms (same style as i2c)
		// For now skeleton may return NotImplemented.
		static Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms);
		static Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms);

		// IRQ entry point used by board-level glue
		static void on_irq_isr();
	};

}  // namespace uart