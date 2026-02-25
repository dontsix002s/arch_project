/**
 * @file  board/rev_a/inc/pin_map.h
 * @brief GPIO pin assignments for revision A of the reference board.
 *
 * Pin indices are expressed as plain integer constants so this header compiles
 * without any MCU vendor SDK.  When integrating the vendor SDK, replace the
 * placeholder values with the actual GPIO_TypeDef pointers and pin bitmasks
 * from stm32h7xx_hal_gpio.h or stm32h7xx_ll_gpio.h.
 */
#pragma once

#include <cstdint>

namespace board {
namespace rev_a {
namespace pins {

// ---------------------------------------------------------------------------
// I2C1 – connected to the Arduino-compatible header CN7 (Nucleo-H743ZI)
// ---------------------------------------------------------------------------

/// GPIOB index (placeholder; replace with a GPIO_TypeDef* in real code).
constexpr uint32_t kGpioB = 1U;

/// I2C1 SCL – PB6, Alternate Function 4 (I2C1_SCL).
constexpr uint32_t kI2c1SclPort = kGpioB;
constexpr uint32_t kI2c1SclPin  = 6U;
constexpr uint32_t kI2c1SclAf   = 4U;  ///< AF4 = I2C1_SCL on STM32H7

/// I2C1 SDA – PB7, Alternate Function 4 (I2C1_SDA).
constexpr uint32_t kI2c1SdaPort = kGpioB;
constexpr uint32_t kI2c1SdaPin  = 7U;
constexpr uint32_t kI2c1SdaAf   = 4U;  ///< AF4 = I2C1_SDA on STM32H7

// ---------------------------------------------------------------------------
// FT6236 touch controller additional signals
// ---------------------------------------------------------------------------

/// GPIOC index (placeholder).
constexpr uint32_t kGpioC = 2U;

/// FT6236 /INT – PC13, active-low interrupt output from the touch IC.
constexpr uint32_t kFt6236IntPort = kGpioC;
constexpr uint32_t kFt6236IntPin  = 13U;

/// FT6236 /RST – PC14, active-low reset input to the touch IC.
constexpr uint32_t kFt6236RstPort = kGpioC;
constexpr uint32_t kFt6236RstPin  = 14U;

}  // namespace pins
}  // namespace rev_a
}  // namespace board
