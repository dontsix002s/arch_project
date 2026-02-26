/**
 * @file  system/inc/system_clocks.h
 * @brief System clock snapshot API.
 *
 * Provides a lightweight, vendor-free snapshot of the current MCU clock tree.
 * The snapshot is updated by the board BSP after any RCC/PLL reconfiguration
 * and can be queried by any subsystem (e.g. timers) that needs the current
 * peripheral bus clock frequencies.
 *
 * Usage
 * -----
 * @code
 * #include "system/inc/system_clocks.h"
 *
 * // Initialise to safe reset defaults (64 MHz HSI):
 * system::clocks::set(system::clocks::reset_defaults());
 *
 * // Read the current snapshot:
 * auto clks = system::clocks::get();
 * uint32_t hz = clks.apb1_timer_hz;
 * @endcode
 */
#pragma once

#include <cstdint>

namespace system::clocks
{

/// Snapshot of the MCU clock tree frequencies (all values in Hz).
struct Clocks
{
    uint32_t sys_hz;         ///< AHB / HCLK frequency (after AHB prescaler).
    uint32_t apb1_hz;        ///< APB1 peripheral clock.
    uint32_t apb1_timer_hz;  ///< APB1 timer clock (= apb1_hz * 2 when APB1 prescaler > 1).
    uint32_t apb2_hz;        ///< APB2 peripheral clock.
    uint32_t apb2_timer_hz;  ///< APB2 timer clock.
};

/// Return a Clocks snapshot representing the MCU reset defaults.
///
/// Assumes the internal 64 MHz HSI oscillator is the clock source with all
/// bus dividers at their reset values (no prescalers).
Clocks reset_defaults();

/// Return the currently stored clock snapshot.
Clocks get();

/// Store a new clock snapshot.
///
/// Called by the BSP after each RCC/PLL reconfiguration so that the rest of
/// the system sees up-to-date frequencies.
void set(const Clocks& clocks);

}  // namespace system::clocks
