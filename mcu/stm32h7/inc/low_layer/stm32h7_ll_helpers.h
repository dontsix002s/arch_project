#pragma once
#include <cstdint>

#ifndef STMFW_VENDOR_HEADERS_AVAILABLE
#define STMFW_VENDOR_HEADERS_AVAILABLE 0
#endif

#if STMFW_VENDOR_HEADERS_AVAILABLE
#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#endif

namespace mcu::stm32h7::ll {

#if STMFW_VENDOR_HEADERS_AVAILABLE

	inline GPIO_TypeDef* gpio_from_port_index(uint32_t idx)
	{
		switch (idx)
		{
		case 0U: return GPIOA;
		case 1U: return GPIOB;
		case 2U: return GPIOC;
		case 3U: return GPIOD;
		case 4U: return GPIOE;
		case 5U: return GPIOF;
		case 6U: return GPIOG;
		case 7U: return GPIOH;
		default: return nullptr;
		}
	}

	inline void enable_gpio_clock(uint32_t idx)
	{
		switch (idx)
		{
		case 0U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA); break;
		case 1U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB); break;
		case 2U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOC); break;
		case 3U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD); break;
		case 4U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE); break;
		case 5U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOF); break;
		case 6U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOG); break;
		case 7U: LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOH); break;
		default: break;
		}
	}

	inline uint32_t pin_mask(uint8_t pin) { return (1U << pin); }

	inline uint32_t ll_gpio_af(uint8_t af)
	{
		switch (af)
		{
		case 7U: return LL_GPIO_AF_7;
		default: return LL_GPIO_AF_0; // extend as needed
		}
	}

#endif

} // namespace mcu::stm32h7::ll