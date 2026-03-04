#pragma once
#include <cstdint>

namespace mcu::stm32h750 {

	// Peripheral base addresses (RM / CMSIS device header)
	inline constexpr uint32_t kUsart1Base = 0x40011000U;
	inline constexpr uint32_t kI2c1Base = 0x40005400U;

	// GPIO base addresses (optional; board layer обычно их не использует напрямую)
	inline constexpr uint32_t kGpioABase = 0x58020000U;
	inline constexpr uint32_t kGpioBBase = 0x58020400U;
	inline constexpr uint32_t kGpioCBase = 0x58020800U;

} // namespace mcu::stm32h750