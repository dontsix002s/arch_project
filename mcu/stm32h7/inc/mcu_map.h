#pragma once

#if defined(STM32H750xx)
#include "mcu/stm32h7/inc/chips/mcu_map_stm32h750.h"
#elif defined(STM32H743xx)
#include "mcu/stm32h7/inc/chips/mcu_map_stm32h743.h"
#else
#error "Unsupported STM32H7 MCU variant: define STM32H750xx or STM32H743xx (etc.)"
#endif