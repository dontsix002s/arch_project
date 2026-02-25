/**
 * @file  app/src/main.cpp
 * @brief Application entry point skeleton.
 *
 * This file demonstrates how the layered architecture composes at the
 * application level.  All headers are included for compilation checking;
 * the actual function calls are left as TODO stubs until backend
 * implementations are provided.
 *
 * Include hierarchy used here (longest-path style as required):
 *   system/inc/system_init.h
 *   board/rev_a/inc/board_config.h        → pulls in LL backend + policy + wait
 *   board/rev_a/inc/pin_map.h
 *   devices/ft6236/inc/ft6236.h           → pulls in i2c_error.h
 *   app/inc/app_config.h
 */

#include "system/inc/system_init.h"
#include "board/rev_a/inc/board_config.h"
#include "board/rev_a/inc/pin_map.h"
#include "devices/ft6236/inc/ft6236.h"
#include "app/inc/app_config.h"

// Convenient type alias for the touch driver on the rev-A board.
using Touch = devices::Ft6236<board::rev_a::I2c1Bus>;

int main() {
    // -----------------------------------------------------------------
    // System and peripheral initialisation
    // TODO: uncomment when backend implementations are provided.
    // -----------------------------------------------------------------

    // system::init();
    // board::rev_a::I2c1Bus::init();
    // Touch::init();

    // -----------------------------------------------------------------
    // Super-loop
    // -----------------------------------------------------------------
    for (;;) {
        // TODO: read touch data and dispatch to application logic.
        // Touch::TouchPoint tp{};
        // if (Touch::read_touch(tp) == i2c::Error::Ok && tp.touch_count > 0) {
        //     // process tp.points[0].x, tp.points[0].y …
        // }
    }
}
