/**
 * @file  system/src/system_init.cpp
 * @brief Placeholder implementation of stmfw::system::init().
 *
 * Replace the TODO comment with real clock, FPU, MPU, and cache
 * configuration calls for your target MCU and BSP.
 */

#include "system/inc/system_init.h"

namespace stmfw::system {

void init() {
    // TODO: enable FPU (SCB->CPACR |= (3UL << 20) | (3UL << 22))
    // TODO: configure PLL and clock tree (HAL_RCC_OscConfig / LL_RCC_*)
    // TODO: set AHB/APB dividers
    // TODO: enable I-Cache and D-Cache (SCB_EnableICache / SCB_EnableDCache)
    // TODO: configure MPU regions
    // TODO: initialise SysTick (HAL_Init / SysTick_Config)
}

}  // namespace stmfw::system
