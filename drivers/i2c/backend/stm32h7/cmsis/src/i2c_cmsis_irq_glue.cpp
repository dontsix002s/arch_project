/**
 * @file  drivers/i2c/backend/stm32h7/cmsis/src/i2c_cmsis_irq_glue.cpp
 * @brief IRQ glue placeholder for the STM32H7 CMSIS I2C backend.
 *
 * This file will define the STM32 IRQ handler entry points and route them to
 * the CMSIS backend's internal handler functions via the PeriphDescriptor
 * handler slots.
 *
 * Steps to implement
 * ------------------
 * 1. Include the CMSIS backend header and the board config.
 * 2. Define the CMSIS-named IRQ handlers with C linkage, delegating via
 *    Periph.IrqEventHandler / Periph.IrqErrorHandler.
 * 3. Inside CmsisBackend::init(), assign the internal handler functions.
 *    For DmaIrqPolicy, also populate DmaTxHandler and DmaRxHandler.
 *
 * No ISR bodies are implemented here yet.
 */

// (placeholder – no compilable code until the CMSIS driver is implemented)
