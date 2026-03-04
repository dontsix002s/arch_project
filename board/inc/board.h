/**
 * @file  board/inc/board.h
 * @brief Single public BSP include for application and device-driver code.
 *
 * This is the only BSP header that application code and device drivers should
 * include.  It pulls in the active revision (selected by
 * board/inc/board_revision_select.h) and re-exports the key APIs under stable
 * `board::` namespace aliases so that no code outside the BSP ever needs to
 * name a specific revision.
 *
 * Exported names
 * --------------
 *   board::init()       – initialise the active board revision
 *   board::time         – namespace alias for board::active::time
 *   board::i2c1         – namespace alias for board::active::i2c1
 *
 * Changing the active revision
 * ----------------------------
 * Edit board/inc/board_revision_select.h (or pass -DBOARD_REVISION=<n> to the
 * compiler).  No other file needs to change.
 */
#pragma once

#include "board/inc/board_revision_select.h"

namespace board {

    /// Initialise the active board revision
    ///
    /// Delegates to board::active::init() which is implemented in the revision's
    /// board_init.cpp.
    inline void init()
    {
        active::init();
    }

    /// Stable namespace alias for the active revision's time API
    namespace time = active::time;

    /// Stable namespace alias for the active revision's I2C1 types.
    namespace i2c1 = active::i2c1;

}