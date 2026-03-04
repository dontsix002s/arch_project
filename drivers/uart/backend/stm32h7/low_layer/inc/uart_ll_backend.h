#pragma once
#include <cstddef>
#include <cstdint>

#include "drivers/uart/core/inc/uart_error.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_periph.h"

namespace uart::backend::stm32h7::ll {

	template <const uart::backend::stm32h7::PeriphDescriptor& Periph, typename Policy>
	struct LlBackend
	{
		static void init()
		{
			// TODO: enable clocks, configure GPIO, configure USART registers
			(void)Periph;
		}

		static void deinit()
		{
			// TODO
			(void)Periph;
		}

		static uart::Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			// TODO: implement polling / irq / dma based on Policy
			(void)buf;
			(void)len;
			(void)timeout_ms;
			(void)Periph;
			return uart::Error::NotImplemented;
		}

		static uart::Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			// TODO
			(void)buf;
			(void)len;
			(void)timeout_ms;
			(void)Periph;
			return uart::Error::NotImplemented;
		}

		static void on_irq_isr()
		{
			// TODO: implement when IrqPolicy is supported
			(void)Periph;
		}
	};

}  // namespace uart::backend::stm32h7::ll