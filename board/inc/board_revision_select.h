/**
 * @file  board/inc/board_revision_select.h
 * @brief Manual board revision selection.
 *
 * This file selects the active board revision by including its export header
 * and defining the `board::active` namespace alias.
 *
 * How to add a new revision
 * -------------------------
 * 1. Create board/rev_<x>/inc/board_rev.h following the pattern in rev_a.
 * 2. Add a new `#elif` block below that includes the new header.
 * 3. Change `BOARD_REVISION` to the new value (or let the build system set it).
 *
 * The application and all drivers include board/inc/board.h which pulls in
 * this header automatically; no other file needs to change.
 */
#pragma once

// ---------------------------------------------------------------------------
// Select active revision
// ---------------------------------------------------------------------------
// Set BOARD_REVISION via the compiler command line (-DBOARD_REVISION=1) or
// leave it unset to default to revision A (value 0).

#ifndef BOARD_REVISION
#  define BOARD_REVISION 0
#endif

#if BOARD_REVISION == 0
#  include "board/rev_a/inc/board_rev.h"
namespace board {
/// Alias that always points to the currently selected revision.
namespace active = rev_a;
}  // namespace board
#else
#  error "Unknown BOARD_REVISION value – add a new block in board_revision_select.h"
#endif
