#pragma once
#include <cstddef>
#include <cstdint>

#include "drivers/uart/core/inc/uart_error.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_periph.h"
#include "mcu/stm32h7/inc/low_layer/stm32h7_ll_helpers.h"

#ifndef STMFW_VENDOR_HEADERS_AVAILABLE
#define STMFW_VENDOR_HEADERS_AVAILABLE 0
#endif

#if STMFW_VENDOR_HEADERS_AVAILABLE
#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx_ll_gpio.h"
#endif

namespace uart::backend::stm32h7::ll {

	template <const uart::backend::stm32h7::PeriphDescriptor& Periph, typename Policy>
	struct LlBackend
	{
#if STMFW_VENDOR_HEADERS_AVAILABLE
		static USART_TypeDef* usart()
		{
			return reinterpret_cast<USART_TypeDef*>(Periph.usart_base);
		}

		static void enable_usart_clock()
		{
			// пока реализуем USART1 (instance_index=0)
			switch (Periph.instance_index)
			{
			case 0U: LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); break;
			default: break;
			}
		}

		static void init()
		{
			// clocks for GPIO ports + USART
			mcu::stm32h7::ll::enable_gpio_clock(Periph.tx.port_index);
			mcu::stm32h7::ll::enable_gpio_clock(Periph.rx.port_index);
			enable_usart_clock();

			// GPIO init TX/RX
			LL_GPIO_InitTypeDef gpio{}
			;
			gpio.Mode = LL_GPIO_MODE_ALTERNATE;
			gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
			gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
			gpio.Pull = LL_GPIO_PULL_UP;

			// TX
			gpio.Pin = mcu::stm32h7::ll::pin_mask(Periph.tx.pin);
			gpio.Alternate = static_cast<uint32_t>(Periph.tx.af); // see note below
			LL_GPIO_Init(mcu::stm32h7::ll::gpio_from_port_index(Periph.tx.port_index), &gpio);

			// RX
			gpio.Pin = mcu::stm32h7::ll::pin_mask(Periph.rx.pin);
			gpio.Alternate = static_cast<uint32_t>(Periph.rx.af);
			LL_GPIO_Init(mcu::stm32h7::ll::gpio_from_port_index(Periph.rx.port_index), &gpio);

			// USART init
			LL_USART_Disable(usart());

			LL_USART_SetTransferDirection(usart(), LL_USART_DIRECTION_TX_RX);
			LL_USART_ConfigCharacter(usart(),
				LL_USART_DATAWIDTH_8B,
				LL_USART_PARITY_NONE,
				LL_USART_STOPBITS_1);
			LL_USART_SetOverSampling(usart(), LL_USART_OVERSAMPLING_16);

			LL_USART_SetBaudRate(usart(), Periph.clock_hz, LL_USART_OVERSAMPLING_16, Periph.baud);

			LL_USART_Enable(usart());
		}

		static void deinit() { LL_USART_Disable(usart()); }

		static uart::Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			(void)timeout_ms; // TODO: add wait strategy later
			for (std::size_t i = 0; i < len; ++i)
			{
				while (!LL_USART_IsActiveFlag_TXE(usart())) {}
				LL_USART_TransmitData8(usart(), buf[i]);
			}
			while (!LL_USART_IsActiveFlag_TC(usart())) {}
			return uart::Error::Ok;
		}

		static uart::Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms)
		{
			(void)timeout_ms;
			for (std::size_t i = 0; i < len; ++i)
			{
				while (!LL_USART_IsActiveFlag_RXNE(usart())) {}
				buf[i] = LL_USART_ReceiveData8(usart());
			}
			return uart::Error::Ok;
		}

		static void on_irq_isr() { /* TODO later */ }

#else
		static void init() {}
		static void deinit() {}
		static uart::Error write(const uint8_t*, std::size_t, uint32_t) { return uart::Error::NotImplemented; }
		static uart::Error read(uint8_t*, std::size_t, uint32_t) { return uart::Error::NotImplemented; }
		static void on_irq_isr() {}
#endif
	};

} // namespace uart::backend::stm32h7::ll