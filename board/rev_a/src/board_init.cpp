/**
 * @file  board/rev_a/src/board_init.cpp
 * @brief Revision-A board initialisation.
 *
 * Implements board::rev_a::init().  Initialisation order:
 *   1. Publish reset-default clock snapshot so that SystemTimer has a valid
 *      frequency before the hardware timer starts.
 *   2. Start SystemTimer on reset clocks so that timeouts work during PLL init.
 *   3. Apply the Performance clock profile (configures HSE + PLL1).
 *   4. Refresh SystemTimer so it runs at the correct rate for the new clocks.
 */

#include "board/rev_a/inc/board_rev.h"
#include "board/rev_a/inc/board_clock.h"
#include "system/inc/system_clocks.h"
#include "system/inc/system_timer.h"

namespace board::rev_a
{
    void init()
    {
        // 1. Publish the reset-default clock snapshot (64 MHz HSI).
        system::clocks::set(system::clocks::reset_defaults());

        // 2. Start the system timebase on reset clocks so the BSP can use
        //    board::rev_a::time::now_ms() during the PLL sequence.
        system::SystemTimer<SystemTimerTraits>::init();

        // 3. Configure RCC / PLL for maximum performance.
        board::rev_a::clock::apply(clock::Profile::Performance);

        // 4. Reconfigure the timebase prescaler for the new APB1 timer clock.
        system::SystemTimer<SystemTimerTraits>::refresh();
    }
}
