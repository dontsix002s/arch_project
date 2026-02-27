/**
 * @file  system/src/system_clocks.cpp
 * @brief System clock snapshot implementation.
 *
 * Implements stmfw::system::clocks::reset_defaults(), get(), and set().
 * The snapshot is stored in a single static variable and updated by the BSP
 * after each RCC/PLL reconfiguration.
 */

#include "system/inc/system_clocks.h"

namespace stmfw::system::clocks
{

namespace
{
    /// Reset-default clock snapshot (64 MHz HSI, no prescalers).
    static constexpr Clocks kResetDefaults = {
        /* sysclk_hz     = */ 64'000'000U,
        /* hclk_hz       = */ 64'000'000U,
        /* apb1_hz       = */ 64'000'000U,
        /* apb1_timer_hz = */ 64'000'000U,
        /* apb2_hz       = */ 64'000'000U,
        /* apb2_timer_hz = */ 64'000'000U,
    };

    /// Internal stored snapshot; initialised to reset defaults.
    static Clocks s_clocks = kResetDefaults;
}  // namespace

Clocks reset_defaults()
{
    return kResetDefaults;
}

Clocks get()
{
    return s_clocks;
}

void set(const Clocks& clocks)
{
    s_clocks = clocks;
}

}  // namespace stmfw::system::clocks
