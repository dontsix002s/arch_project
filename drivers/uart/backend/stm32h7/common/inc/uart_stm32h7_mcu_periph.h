#pragma once

// -- Includes ------------------------------------
#include <cstdint>

namespace uart::backend::stm32h7 {

	struct McuPeriph
	{
		uint32_t usart_base;
		uint8_t  instance_index; // 0=USART1, 1=USART2...
	};

}