/**
 * @file  board/rev_a/src/board_timebase.cpp
 * @brief Revision-A board timebase implementation.
 *
 * Implements board::rev_a::time::now_ms() and TimebaseClock::now_ms() by
 * delegating to system::SystemTimer<SystemTimerTraits>::ticks().
 */

#include "board/rev_a/inc/board_timebase.h"
#include "system/inc/system_timer.h"

namespace board {
namespace rev_a {
namespace time {

uint32_t now_ms() {
    return system::SystemTimer<board::rev_a::SystemTimerTraits>::ticks();
}

uint32_t TimebaseClock::now_ms() {
    return board::rev_a::time::now_ms();
}

}  // namespace time
}  // namespace rev_a
}  // namespace board
