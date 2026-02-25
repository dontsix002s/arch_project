/**
 * @file  drivers/i2c/backend/stm32h7/ll/src/i2c_ll_irq_glue.cpp
 * @brief IRQ glue placeholder for the STM32H7 LL I2C backend.
 *
 * This file will define the STM32 IRQ handler entry points and route them to
 * the backend's internal handler functions via the PeriphDescriptor slots.
 *
 * Steps to implement
 * ------------------
 * 1. Include the LL backend header and the board config:
 *    @code
 *    #include "drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h"
 *    #include "board/rev_a/inc/board_config.h"
 *    @endcode
 *
 * 2. Define the STM32 CMSIS-named IRQ handlers (must have C linkage):
 *    @code
 *    extern "C" void I2C1_EV_IRQHandler() {
 *        if (board::rev_a::kI2c1Periph.IrqEventHandler) {
 *            board::rev_a::kI2c1Periph.IrqEventHandler();
 *        }
 *    }
 *    extern "C" void I2C1_ER_IRQHandler() {
 *        if (board::rev_a::kI2c1Periph.IrqErrorHandler) {
 *            board::rev_a::kI2c1Periph.IrqErrorHandler();
 *        }
 *    }
 *    @endcode
 *
 * 3. Inside LlBackend::init(), assign the internal handler functions:
 *    @code
 *    kI2c1Periph.IrqEventHandler = &ll_i2c1_event_handler;
 *    kI2c1Periph.IrqErrorHandler = &ll_i2c1_error_handler;
 *    // DMA slots if DmaIrqPolicy is selected:
 *    kI2c1Periph.DmaTxHandler = &ll_i2c1_dma_tx_handler;
 *    kI2c1Periph.DmaRxHandler = &ll_i2c1_dma_rx_handler;
 *    @endcode
 *
 * No ISR bodies are implemented here yet.
 */

// (placeholder – no compilable code until the LL driver is implemented)
