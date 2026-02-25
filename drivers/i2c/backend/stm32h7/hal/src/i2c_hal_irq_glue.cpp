/**
 * @file  drivers/i2c/backend/stm32h7/hal/src/i2c_hal_irq_glue.cpp
 * @brief IRQ glue placeholder for the STM32H7 HAL I2C backend.
 *
 * This file will define the STM32 IRQ handler entry points and route them to
 * HAL_I2C_EV_IRQHandler / HAL_I2C_ER_IRQHandler via the PeriphDescriptor
 * handler slots.
 *
 * Steps to implement
 * ------------------
 * 1. Include the HAL backend header and the board config.
 * 2. Define the CMSIS-named IRQ handlers with C linkage, delegating to
 *    the HAL callbacks via Periph.IrqEventHandler / Periph.IrqErrorHandler.
 * 3. Inside HalBackend::init(), assign the internal handler functions to
 *    Periph.IrqEventHandler and Periph.IrqErrorHandler.
 *    For DmaIrqPolicy, also populate DmaTxHandler and DmaRxHandler.
 *
 * No ISR bodies are implemented here yet.
 */

// (placeholder – no compilable code until the HAL driver is implemented)
