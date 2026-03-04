#pragma once
#include <cstdint>

//#include "stm32h7xx.h"

namespace uart::backend::stm32h7 {

	struct PeriphDescriptor
	{
		uint32_t  base_address; // e.g. USART1 base: 0x40011000 (STM32H743)
		uint8_t   instance_index; // 0=USART1, 1=USART2, ...
		uint32_t  clock_hz; // peripheral input clock (APB1/APB2), placeholder ok for now
		uint32_t  baud;

		// Later we’ll add pins/clocks/irqn as needed.
	};

}