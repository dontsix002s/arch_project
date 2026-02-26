/**
 * @file  board/rev_a/inc/board_rev.h
 * @brief Revision-A BSP export header.
 *
 * Includes all revision-A exports (board_config, timebase, timer traits) and
 * declares `board::rev_a::init()`.
 *
 * This header is included by board/inc/board_revision_select.h to expose the
 * full revision-A API under the `board::active` namespace alias.
 *
 * Application / device code must NOT include this header directly.
 * Use board/inc/board.h instead.
 */
#pragma once

#include "board/rev_a/inc/board_config.h"
#include "board/rev_a/inc/board_timebase.h"
#include "board/rev_a/inc/board_system_timer_traits.h"

namespace board {
namespace rev_a {

/// Initialise the revision-A board hardware.
///
/// Must be called once at startup before any peripheral or driver is used.
/// At minimum this starts the board timebase (TIM7, 1 kHz tick).
/// Implemented in board/rev_a/src/board_init.cpp.
void init();

}  // namespace rev_a
}  // namespace board
