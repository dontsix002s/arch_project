/**
 * @file  app/src/main.cpp
 * @brief Minimal bring-up entry point for STM32H750VB.
 *
 * Initialisation sequence:
 *   1. system::init()         – FPU, clocks, caches, MPU
 *   2. board::init()          – TIM7 1 kHz timebase, peripherals
 *   3. system::swo::init()    – SWO/ITM logger (must come after board::init()
 *                               because board::init() changes the clock tree)
 */

#include "system/inc/system_init.h"
#include "board/inc/board.h"
#include "system/inc/system_swo.h"

int main() {
    system::init();
    board::init();

    // SWO init after board::init() – clock tree must be stable first.
    // 480 MHz = Performance-profile core clock for STM32H750.
    system::swo::init(480'000'000U);

    system::swo::write("boot\n");
    system::swo::write("board init ok\n");

    for (;;) {}
}
