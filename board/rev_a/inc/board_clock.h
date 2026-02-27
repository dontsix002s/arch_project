/**
 * @file  board/rev_a/inc/board_clock.h
 * @brief Board clock profile API for revision A.
 *
 * Provides a vendor-free interface for selecting a board clock profile.
 * The implementation (board_clock_stm32h7_ll.cpp) contains all vendor-specific
 * RCC/PLL configuration and must NOT be included in this header.
 *
 * Application / device code must NOT include this header directly.
 * Use board/inc/board.h instead.
 */
#pragma once

namespace stmfw::board::rev_a::clock
{

/// Available board clock profiles.
enum class Profile
{
    Performance,  ///< Maximum performance (480 MHz system clock on STM32H750).
    Balanced,     ///< Balanced performance / power.
    LowPower,     ///< Minimum clock rates for low-power operation.
};

/// Apply the requested clock profile.
///
/// Configures the RCC oscillators, PLL, and bus dividers for the requested
/// profile and publishes the resulting frequencies via stmfw::system::clocks::set().
///
/// @param profile  The desired clock profile.
/// @return true if the profile was applied successfully, false otherwise.
bool apply(Profile profile);

}  // namespace stmfw::board::rev_a::clock
