#pragma once
#include <cstddef>
#include <cstdint>

#include "stm32h7xx.h" // for USART_TypeDef (type only)
#include "drivers/uart/core/inc/uart_error.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_periph.h"

namespace uart::backend::stm32h7::ll {

	template <const uart::backend::stm32h7::PeriphDescriptor& Periph, typename Policy>
	struct LlBackend
	{
		///--------------------------------------------------------------------
		/// 
		///--------------------------------------------------------------------
		static USART_TypeDef* instance()
		{
			return reinterpret_cast<USART_TypeDef*>(Periph.base_address);
		}

		///--------------------------------------------------------------------
		///
		///--------------------------------------------------------------------
		static void init()
		{
			// TODO: clocks/gpio/usart registers
			(void)instance();
		}

		///--------------------------------------------------------------------
		///
		///--------------------------------------------------------------------
		static void deinit()
		{
			// TODO
		}

		///--------------------------------------------------------------------
		///
		///--------------------------------------------------------------------
		static uart::Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			(void)buf; (void)len; (void)timeout_ms;
			return uart::Error::NotImplemented;
		}

		///--------------------------------------------------------------------
		///
		///--------------------------------------------------------------------
		static uart::Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			(void)buf; (void)len; (void)timeout_ms;
			return uart::Error::NotImplemented;
		}

		///--------------------------------------------------------------------
		///
		///--------------------------------------------------------------------
		static void on_irq_isr()
		{
			// TODO when IrqPolicy supported
		}
	};
}