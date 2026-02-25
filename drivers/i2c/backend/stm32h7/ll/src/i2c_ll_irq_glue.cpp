/**
 * @file  drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp
 * @brief Safe stub IRQ handlers for the STM32H7 LL I2C1 backend.
 *
 * Provides CMSIS-named IRQ handler entry points for I2C1.  Each handler
 * checks the corresponding slot in board::rev_a::kI2c1Periph and calls it
 * if non-null, otherwise returns immediately.
 *
 * No real IRQ-driven I2C logic is implemented here; these are safe stubs that
 * allow the translation unit to compile and do nothing in polling mode.
 */

#include "board/rev_a/inc/board_config.h"

extern "C" void I2C1_EV_IRQHandler()
{
    if (board::rev_a::kI2c1Periph.IrqEventHandler) {
        board::rev_a::kI2c1Periph.IrqEventHandler();
    }
}

extern "C" void I2C1_ER_IRQHandler()
{
    if (board::rev_a::kI2c1Periph.IrqErrorHandler) {
        board::rev_a::kI2c1Periph.IrqErrorHandler();
    }
}
