#pragma once

// -- Includes --------------------------
#include <cstdint>
#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"

// включать только в *ll_backend_impl.h !!!

namespace mcu::stm32h7::ll {

	///------------------------------------------------------------------------
	///  gpio_from_port_index()
	///------------------------------------------------------------------------
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

	///------------------------------------------------------------------------
	///  enable_gpio_clock()
	///------------------------------------------------------------------------
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

	///------------------------------------------------------------------------
	///  ll_gpio_af()
	///------------------------------------------------------------------------
	inline uint32_t ll_gpio_af(uint8_t af)
	{
		switch (af)
		{
		case 0:  return (LL_GPIO_AF_0);
		case 1:  return (LL_GPIO_AF_1);
		case 2:  return (LL_GPIO_AF_2);
		case 3:  return (LL_GPIO_AF_3);
		case 4:  return (LL_GPIO_AF_4);
		case 5:  return (LL_GPIO_AF_5);
		case 6:  return (LL_GPIO_AF_6);
		case 7:  return (LL_GPIO_AF_7);
		case 8:  return (LL_GPIO_AF_8);
		case 9:  return (LL_GPIO_AF_9);
		case 10: return (LL_GPIO_AF_10);
		case 11: return (LL_GPIO_AF_11);
		case 12: return (LL_GPIO_AF_12);
		case 13: return (LL_GPIO_AF_13);
		case 14: return (LL_GPIO_AF_14);
		case 15: return (LL_GPIO_AF_15);
		default: return LL_GPIO_AF_0; // extend as needed //???????
		}
	}

	///------------------------------------------------------------------------
	///  pin_mask()
	///------------------------------------------------------------------------
	inline uint32_t pin_mask(uint8_t pin)
	{
		return (1U << pin);
	}
}