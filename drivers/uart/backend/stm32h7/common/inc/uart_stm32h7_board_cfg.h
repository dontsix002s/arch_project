#pragma once

// -- Includes --------------------------
#include <cstdint>

namespace uart::backend::stm32h7 {

	///--------------------------------------------
	struct PinAf {
		uint32_t port_index;
		uint8_t  pin;
		uint8_t  af;
	};

	///--------------------------------------------
	struct BoardCfg {
		uint32_t clock_hz;
		uint32_t baud;
		PinAf    tx;
		PinAf    rx;
	};
}