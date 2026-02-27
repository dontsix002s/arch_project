/**
 * @file  system/inc/system_swo.h
 * @brief Minimal SWO/ITM printf-style logger API for STM32H750 bring-up.
 *
 * No vendor or CMSIS headers are included here; all MCU-specific code is
 * confined to system/src/system_swo.cpp.
 *
 * Usage:
 *   stmfw::system::init();
 *   board::init();                          // must run first – changes clocks
 *   stmfw::system::swo::init(480'000'000U);        // Performance-profile core clock
 *   stmfw::system::swo::write("boot\n");
 */
#pragma once

#include <cstdint>

namespace stmfw::system::swo {

/// Initialise the SWO/ITM subsystem.
///
/// @param cpu_hz  Current CPU frequency in Hz (used to configure TPI->ACPR).
///                Call after board::init() so the clock tree is stable.
void init(uint32_t cpu_hz);

/// Write a single character to ITM stimulus port 0.
void putc(char c);

/// Write a NUL-terminated string to ITM stimulus port 0.
void write(const char* s);

}  // namespace stmfw::system::swo
