/**
 * @file  board/rev_a/inc/board_timebase.h
 * @brief Public timebase API for board revision A.
 *
 * Provides `stmfw::board::rev_a::time::now_ms()` and the `TimebaseClock` helper
 * struct that satisfies the Clock contract used by i2c::wait::TimedBusyWait.
 *
 * Application / device code must NOT include this header directly.
 * Use board/inc/board.h instead.
 */
#pragma once

#include <cstdint>

#include "system/inc/system_timer.h"
#include "board/rev_a/inc/board_system_timer_traits.h"

namespace stmfw::board::rev_a::time
{

    /// Return the current system tick count in milliseconds.
    ///
    /// Implemented in board/rev_a/src/board_timebase.cpp.
    uint32_t now_ms();

    /// Clock type satisfying the `Clock` contract for TimedBusyWait.
    ///
    /// Pass this type as the Clock template argument:
    /// @code
    /// using Wait = i2c::wait::TimedBusyWait<stmfw::board::rev_a::time::TimebaseClock>;
    /// @endcode
    struct TimebaseClock
    {
        static uint32_t now_ms();
    };
}
