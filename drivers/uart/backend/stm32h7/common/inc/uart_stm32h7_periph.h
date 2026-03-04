#pragma once
#include <cstdint>

//#include "stm32h7xx.h"

namespace uart::backend::stm32h7 {

	struct PinAf
	{
		uint32_t port_index;
		uint8_t  pin;
		uint8_t  af;
	};

	struct PeriphDescriptor
	{
		uint32_t usart_base;
		uint8_t  instance_index;
		uint32_t clock_hz;
		uint32_t baud;

		PinAf tx;
		PinAf rx;
	};
}