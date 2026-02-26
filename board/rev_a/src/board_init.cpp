/**
 * @file  board/rev_a/src/board_init.cpp
 * @brief Revision-A board initialisation.
 *
 * Implements board::rev_a::init().  At minimum this starts the board
 * timebase (TIM7, 1 kHz tick).  Add further peripheral initialisations here
 * as the board support package grows.
 */

#include "board/rev_a/inc/board_rev.h"
#include "system/inc/system_timer.h"

namespace board::rev_a
{
    void init()
    {
        // Start the 1 kHz system timebase (TIM7).
        system::SystemTimer<SystemTimerTraits>::init();
    }
}
