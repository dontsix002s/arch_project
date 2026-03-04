#pragma once
#include <cstdint>

#include "stm32h7xx.h"

namespace uart::backend::stm32h7 {

	struct PeriphDescriptor
	{
		USART_TypeDef* instance;
		uint8_t        instance_index; // 0=USART1, 1=USART2... (Í‡Í ‚ i2c)

		uint32_t baud;

		// Later weíll add pins/clocks/irqn as needed.
	};

}  // namespace uart::backend::stm32h7