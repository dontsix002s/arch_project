/**
 * @file  system/inc/system_init.h
 * @brief Early MCU system initialisation API.
 *
 * stmfw::system::init() must be called before any peripheral driver or RTOS service
 * is used.  It sets up the clock tree, enables the FPU, configures the MPU
 * and caches, and initialises the SysTick / RTOS tick source.
 */
#pragma once

namespace stmfw::system {

/// Perform early MCU system initialisation.
///
/// Responsibilities (to be implemented for the target MCU):
///   - Enable and configure the PLL / clock tree.
///   - Set AHB / APB bus dividers.
///   - Enable the FPU (Cortex-M7 / M4 / M33).
///   - Configure the instruction and data caches.
///   - Set up the MPU regions.
///   - Initialise the SysTick tick source (bare-metal) or start the RTOS
///     tick (ThreadX / FreeRTOS).
void init();

}  // namespace stmfw::system
