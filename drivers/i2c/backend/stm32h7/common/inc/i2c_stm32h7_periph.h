/**
 * @file  drivers/i2c/backend/stm32h7/common/inc/i2c_stm32h7_periph.h
 * @brief STM32H7 I2C peripheral descriptor with static ISR/DMA handler slots.
 *
 * One constant PeriphDescriptor instance is declared per physical I2C
 * peripheral in the board configuration file (board/rev_a/inc/board_config.h).
 * All three backend types (LL, HAL, CMSIS) receive the descriptor as a
 * non-type template parameter so the compiler can propagate the base address
 * and handler pointers as compile-time constants.
 *
 * Handler slots
 * -------------
 * The four handler function pointers are initially nullptr.  The IRQ glue
 * translation unit for the selected backend
 *   (drivers/i2c/backend/stm32h7/<type>/src/i2c_irq_glue.cpp)
 * assigns concrete ISR-delegate functions to these slots at startup (typically
 * inside Backend::init()).  The weak default ISR stubs defined by the linker
 * script then forward to these pointers.
 *
 *   IrqEventHandler  –  called from I2C1_EV_IRQHandler / I2C2_EV_IRQHandler …
 *   IrqErrorHandler  –  called from I2C1_ER_IRQHandler / I2C2_ER_IRQHandler …
 *   DmaTxHandler     –  called from DMA Tx stream/channel IRQHandler
 *   DmaRxHandler     –  called from DMA Rx stream/channel IRQHandler
 *
 * Hybrid-backend note
 * -------------------
 * A backend may populate only the slots it uses.  For example, a LL+polling
 * backend leaves all four pointers nullptr, while an LL+DMA backend populates
 * all four.  An HAL+IRQ backend may populate only IrqEventHandler and
 * IrqErrorHandler, leaving the DMA slots nullptr.
 */
#pragma once

#include <cstdint>

namespace i2c {
namespace backend {
namespace stm32h7 {

/// Pointer to a no-argument, no-return ISR delegate function.
using HandlerFn = void (*)();

/// Static descriptor for one physical I2C peripheral instance.
///
/// Declare one constexpr instance per peripheral in the board config and pass
/// it as a non-type template argument to the backend:
///
/// @code
/// inline constexpr PeriphDescriptor kI2c1Periph = {
///     .base_address   = 0x40005400U,
///     .instance_index = 0,
///     .clock_hz       = 100'000'000U,
/// };
///
/// using I2c1LlBackend = i2c::backend::stm32h7::ll::LlBackend<
///     kI2c1Periph,
///     i2c::policy::PollingPolicy,
///     i2c::wait::BaremetalWait>;
/// @endcode
struct PeriphDescriptor {
    /// Peripheral base address in the MCU memory map.
    /// Example values (STM32H743):
    ///   I2C1 = 0x40005400, I2C2 = 0x40005800, I2C3 = 0x40005C00
    uintptr_t   base_address;

    /// Zero-based instance index (0 = I2C1, 1 = I2C2, …).
    /// Used by the backend to select the correct IRQ numbers and DMA channels.
    uint8_t     instance_index;

    /// Peripheral input clock in Hz (APB1 clock for STM32H7 I2C peripherals).
    /// Used by the backend to calculate timing register values.
    /// Typical value: 100 000 000 (100 MHz on a default STM32H743 clock tree).
    uint32_t    clock_hz;

    // -------------------------------------------------------------------------
    // ISR / DMA handler slots
    // -------------------------------------------------------------------------
    // Initially nullptr; assigned by the IRQ glue .cpp during Backend::init().

    /// Delegate for the I2C event ISR (TC, ADDR, RXNE, TXE, …).
    HandlerFn   IrqEventHandler;

    /// Delegate for the I2C error ISR (BERR, ARLO, NACKF, …).
    HandlerFn   IrqErrorHandler;

    /// Delegate for the DMA Tx stream/channel transfer-complete ISR.
    HandlerFn   DmaTxHandler;

    /// Delegate for the DMA Rx stream/channel transfer-complete ISR.
    HandlerFn   DmaRxHandler;
};

}  // namespace stm32h7
}  // namespace backend
}  // namespace i2c
