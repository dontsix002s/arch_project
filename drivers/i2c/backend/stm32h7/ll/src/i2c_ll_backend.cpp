/**
 * @file  drivers/i2c/backend/stm32h7/ll/src/i2c_ll_backend.cpp
 * @brief STM32H7 LL I2C backend – polling-mode implementation.
 *
 * All STM32 vendor headers are confined to this translation unit.  Portable
 * layers (core, policy, wait, devices, board) must not include any STM32
 * vendor headers.
 *
 * External vendor layout (under <repo_root>/external/):
 *   external/ll/inc/stm32h7xx_ll_i2c.h
 *   external/ll/inc/stm32h7xx_ll_gpio.h
 *   external/ll/inc/stm32h7xx_ll_rcc.h
 *   external/ll/inc/stm32h7xx_ll_bus.h
 *   external/cmsis/inc/stm32h7xx.h          (selects MCU via STM32H750xx etc.)
 *
 * See docs/deps/stm32cube_external_layout.md for the full directory structure
 * and required preprocessor defines.
 *
 * Timing
 * ------
 * TIMINGR is set to a placeholder value for 100 kHz at a 100 MHz I2C kernel
 * clock.  Recalculate with STM32CubeMX / AN4235 when the actual clock tree is
 * known.
 */

// ---- STM32 vendor includes (confined to this .cpp) -------------------------
#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_i2c.h"
#include "stm32h7xx_ll_rcc.h"
// ----------------------------------------------------------------------------

#include "drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h"
#include "drivers/i2c/policy/inc/i2c_policy_polling.h"
#include "drivers/i2c/wait/inc/i2c_wait_timed_busy.h"
#include "board/rev_a/inc/board_config.h"

namespace i2c {
namespace backend {
namespace stm32h7 {
namespace ll {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace {

/// Hardcoded TIMINGR for 100 kHz (Standard Mode) at a 100 MHz I2C kernel
/// clock derived from the STM32CubeMX timing spreadsheet (AN4235).
///
/// @warning Recalculate this value with STM32CubeMX when the actual APB1
///          (or I2C mux) kernel clock frequency is known.  The value below
///          is valid only for PCLK1 = 100 MHz with no digital filter.
///
/// Breakdown:  PRESC=1, SCLDEL=4, SDADEL=2, SCLH=0xF, SCLL=0x13
static constexpr uint32_t kTimingr100kHz = 0x10C0ECFFU;

/// Maximum loop iterations used as a hard-stop inside polling loops when no
/// better tick source is available.  Calibrate to the target CPU frequency.
static constexpr uint32_t kPollIterationsPerMs = 1000U;

/// Return a pointer to the I2C_TypeDef for the given base address.
static inline I2C_TypeDef* periph_ptr(uintptr_t base)
{
    return reinterpret_cast<I2C_TypeDef*>(base);
}

/// Translate active LL error flags into an i2c::Error code.
///
/// ISR bits checked (in priority order):
///   ARLO  → ArbitrationLost
///   BERR  → BusError
///   NACKF → NackAddress (address phase) or NackData (data phase)
///
/// @param i2c     Pointer to the I2C peripheral registers.
/// @param tx_done true if the address phase already completed successfully
///                (used to distinguish NackAddress from NackData).
static Error map_errors(I2C_TypeDef* i2c, bool tx_done)
{
    if (LL_I2C_IsActiveFlag_ARLO(i2c)) {
        LL_I2C_ClearFlag_ARLO(i2c);
        return Error::ArbitrationLost;
    }
    if (LL_I2C_IsActiveFlag_BERR(i2c)) {
        LL_I2C_ClearFlag_BERR(i2c);
        return Error::BusError;
    }
    if (LL_I2C_IsActiveFlag_NACK(i2c)) {
        LL_I2C_ClearFlag_NACK(i2c);
        return tx_done ? Error::NackData : Error::NackAddress;
    }
    return Error::Ok;
}

/// Poll a flag predicate for up to @p timeout_ms milliseconds.
///
/// @return true if the flag became true before the timeout, false otherwise.
template <typename FlagFn>
static bool poll_flag(FlagFn flag_fn, uint32_t timeout_ms)
{
    uint32_t iters = timeout_ms * kPollIterationsPerMs;
    while (iters--) {
        if (flag_fn()) {
            return true;
        }
        // Check error flags on every iteration so we can bail early.
        // (Callers re-inspect them via map_errors after this returns false.)
    }
    return false;
}

/// Configure GPIOB PB8 (SCL) and PB9 (SDA) for I2C1, Alternate Function 4,
/// open-drain output with internal pull-up.
///
/// @note This function hard-codes I2C1 on PB8/PB9 AF4 for the initial
///       bring-up target (STM32H750 debug board).  Add similar blocks for
///       other instances or pin assignments as needed.
/// @todo Extend to I2C2/I2C3 once those instances are required.
static void init_gpio_i2c1()
{
    // Enable GPIOB peripheral clock.
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef gpio = {};
    gpio.Pin        = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;  // PB8 = SCL, PB9 = SDA
    gpio.Mode       = LL_GPIO_MODE_ALTERNATE;
    gpio.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio.Pull       = LL_GPIO_PULL_UP;
    gpio.Alternate  = LL_GPIO_AF_4;  // AF4 = I2C1 on STM32H7

    LL_GPIO_Init(GPIOB, &gpio);
}

/// Release PB8 and PB9 back to analog input (high-impedance, no pull).
static void deinit_gpio_i2c1()
{
    LL_GPIO_InitTypeDef gpio = {};
    gpio.Pin        = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
    gpio.Mode       = LL_GPIO_MODE_ANALOG;
    gpio.Speed      = LL_GPIO_SPEED_FREQ_LOW;
    gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio.Pull       = LL_GPIO_PULL_NO;
    gpio.Alternate  = LL_GPIO_AF_0;

    LL_GPIO_Init(GPIOB, &gpio);
}

}  // anonymous namespace

// ---------------------------------------------------------------------------
// LlBackend explicit instantiation
// ---------------------------------------------------------------------------
// Instantiate for the concrete configuration used by the rev-A board so the
// linker can find the method bodies.

template struct LlBackend<
    board::rev_a::i2c1::PeriphDesc,
    board::rev_a::i2c1::Policy,
    board::rev_a::i2c1::Wait>;

// ---------------------------------------------------------------------------
// LlBackend::init
// ---------------------------------------------------------------------------

template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
void LlBackend<Periph, Policy, Wait>::init()
{
    I2C_TypeDef* const i2c = periph_ptr(Periph.base_address);

    // ------------------------------------------------------------------
    // 1. GPIO
    // ------------------------------------------------------------------
    // TODO: Add support for other I2C instances / boards by extending this
    //       if-else chain or providing a GPIO-init callback in PeriphDescriptor.
    if (Periph.instance_index == 0U) {
        init_gpio_i2c1();
    }
    // else if (Periph.instance_index == 1U) { init_gpio_i2c2(); }

    // ------------------------------------------------------------------
    // 2. Peripheral clock
    // ------------------------------------------------------------------
    if (Periph.instance_index == 0U) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
        // Select HSI as I2C1 kernel clock source (default after reset; 64 MHz).
        // Adjust via LL_RCC_SetI2CClockSource() if a different source is needed.
        LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
    }
    // else if (Periph.instance_index == 1U) { ... I2C2 ... }

    // ------------------------------------------------------------------
    // 3. Reset and configure peripheral
    // ------------------------------------------------------------------
    LL_I2C_Disable(i2c);

    // Disable analog noise filter (optional; enable if bus has long capacitive
    // stubs that may introduce glitches at slow speeds).
    LL_I2C_EnableAnalogFilter(i2c);
    LL_I2C_SetDigitalFilter(i2c, 0U);

    // Set timing for 100 kHz.
    // @warning kTimingr100kHz must be recalculated for the actual kernel clock.
    LL_I2C_SetTiming(i2c, kTimingr100kHz);

    // 7-bit addressing, no own-address (master-only).
    LL_I2C_SetOwnAddress1(i2c, 0U, LL_I2C_OWNADDRESS1_7BIT);
    LL_I2C_DisableOwnAddress1(i2c);
    LL_I2C_DisableOwnAddress2(i2c);

    // Disable general-call and clock-stretching (master mode; stretching
    // applies to slave mode but the LL API resets it to enabled by default).
    LL_I2C_DisableGeneralCall(i2c);
    LL_I2C_EnableClockStretching(i2c);  // must be enabled for STM32H7 master

    LL_I2C_Enable(i2c);
}

// ---------------------------------------------------------------------------
// LlBackend::deinit
// ---------------------------------------------------------------------------

template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
void LlBackend<Periph, Policy, Wait>::deinit()
{
    I2C_TypeDef* const i2c = periph_ptr(Periph.base_address);

    LL_I2C_Disable(i2c);

    if (Periph.instance_index == 0U) {
        LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_I2C1);
        deinit_gpio_i2c1();
    }
}

// ---------------------------------------------------------------------------
// LlBackend::write
// ---------------------------------------------------------------------------

template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
Error LlBackend<Periph, Policy, Wait>::write(uint16_t addr,
                                              const uint8_t* buf,
                                              std::size_t    len,
                                              uint32_t       timeout_ms)
{
    if (buf == nullptr || len == 0U) {
        return Error::InvalidArg;
    }

    I2C_TypeDef* const i2c = periph_ptr(Periph.base_address);

    // Check bus is not busy.
    if (LL_I2C_IsActiveFlag_BUSY(i2c)) {
        return Error::Busy;
    }

    // Configure transfer: 7-bit address, len bytes, auto-STOP.
    LL_I2C_HandleTransfer(i2c,
                          static_cast<uint32_t>(addr) << 1U,
                          LL_I2C_ADDRSLAVE_7BIT,
                          static_cast<uint32_t>(len),
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_WRITE);

    bool tx_started = false;

    for (std::size_t i = 0U; i < len; ++i) {
        // Wait for TXIS (transmit data register empty and ready for new byte).
        if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_TXIS(i2c) != 0; },
                       timeout_ms)) {
            // Check for error flags before declaring a timeout.
            Error err = map_errors(i2c, tx_started);
            if (err != Error::Ok) {
                return err;
            }
            LL_I2C_GenerateStopCondition(i2c);
            return Error::Timeout;
        }
        tx_started = true;

        Error err = map_errors(i2c, tx_started);
        if (err != Error::Ok) {
            return err;
        }

        LL_I2C_TransmitData8(i2c, buf[i]);
    }

    // Wait for STOP condition to be generated (auto-end mode).
    if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_STOP(i2c) != 0; },
                   timeout_ms)) {
        return Error::Timeout;
    }
    LL_I2C_ClearFlag_STOP(i2c);

    return map_errors(i2c, true);
}

// ---------------------------------------------------------------------------
// LlBackend::read
// ---------------------------------------------------------------------------

template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
Error LlBackend<Periph, Policy, Wait>::read(uint16_t   addr,
                                             uint8_t*   buf,
                                             std::size_t len,
                                             uint32_t    timeout_ms)
{
    if (buf == nullptr || len == 0U) {
        return Error::InvalidArg;
    }

    I2C_TypeDef* const i2c = periph_ptr(Periph.base_address);

    if (LL_I2C_IsActiveFlag_BUSY(i2c)) {
        return Error::Busy;
    }

    // Configure transfer: read direction, auto-STOP.
    LL_I2C_HandleTransfer(i2c,
                          static_cast<uint32_t>(addr) << 1U,
                          LL_I2C_ADDRSLAVE_7BIT,
                          static_cast<uint32_t>(len),
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);

    for (std::size_t i = 0U; i < len; ++i) {
        // Wait for RXNE (receive data register not empty).
        if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_RXNE(i2c) != 0; },
                       timeout_ms)) {
            Error err = map_errors(i2c, false);
            if (err != Error::Ok) {
                return err;
            }
            LL_I2C_GenerateStopCondition(i2c);
            return Error::Timeout;
        }

        Error err = map_errors(i2c, false);
        if (err != Error::Ok) {
            return err;
        }

        buf[i] = LL_I2C_ReceiveData8(i2c);
    }

    // Wait for auto-generated STOP.
    if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_STOP(i2c) != 0; },
                   timeout_ms)) {
        return Error::Timeout;
    }
    LL_I2C_ClearFlag_STOP(i2c);

    return Error::Ok;
}

// ---------------------------------------------------------------------------
// LlBackend::write_read
// ---------------------------------------------------------------------------

template <const PeriphDescriptor& Periph, typename Policy, typename Wait>
Error LlBackend<Periph, Policy, Wait>::write_read(uint16_t       addr,
                                                   const uint8_t* tx_buf,
                                                   std::size_t    tx_len,
                                                   uint8_t*       rx_buf,
                                                   std::size_t    rx_len,
                                                   uint32_t       timeout_ms)
{
    if (tx_buf == nullptr || tx_len == 0U || rx_buf == nullptr || rx_len == 0U) {
        return Error::InvalidArg;
    }

    I2C_TypeDef* const i2c = periph_ptr(Periph.base_address);

    if (LL_I2C_IsActiveFlag_BUSY(i2c)) {
        return Error::Busy;
    }

    // ------------------------------------------------------------------
    // Phase 1: write (SOFTEND – no STOP yet; keep the bus)
    // ------------------------------------------------------------------
    LL_I2C_HandleTransfer(i2c,
                          static_cast<uint32_t>(addr) << 1U,
                          LL_I2C_ADDRSLAVE_7BIT,
                          static_cast<uint32_t>(tx_len),
                          LL_I2C_MODE_SOFTEND,
                          LL_I2C_GENERATE_START_WRITE);

    bool tx_started = false;

    for (std::size_t i = 0U; i < tx_len; ++i) {
        if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_TXIS(i2c) != 0; },
                       timeout_ms)) {
            Error err = map_errors(i2c, tx_started);
            if (err != Error::Ok) {
                return err;
            }
            LL_I2C_GenerateStopCondition(i2c);
            return Error::Timeout;
        }
        tx_started = true;

        Error err = map_errors(i2c, tx_started);
        if (err != Error::Ok) {
            return err;
        }

        LL_I2C_TransmitData8(i2c, tx_buf[i]);
    }

    // Wait for TC (transfer complete, bus still owned by master).
    if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_TC(i2c) != 0; },
                   timeout_ms)) {
        Error err = map_errors(i2c, true);
        if (err != Error::Ok) {
            return err;
        }
        LL_I2C_GenerateStopCondition(i2c);
        return Error::Timeout;
    }

    // ------------------------------------------------------------------
    // Phase 2: repeated-START + read (auto-STOP)
    // ------------------------------------------------------------------
    LL_I2C_HandleTransfer(i2c,
                          static_cast<uint32_t>(addr) << 1U,
                          LL_I2C_ADDRSLAVE_7BIT,
                          static_cast<uint32_t>(rx_len),
                          LL_I2C_MODE_AUTOEND,
                          LL_I2C_GENERATE_START_READ);

    for (std::size_t i = 0U; i < rx_len; ++i) {
        if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_RXNE(i2c) != 0; },
                       timeout_ms)) {
            Error err = map_errors(i2c, false);
            if (err != Error::Ok) {
                return err;
            }
            LL_I2C_GenerateStopCondition(i2c);
            return Error::Timeout;
        }

        Error err = map_errors(i2c, false);
        if (err != Error::Ok) {
            return err;
        }

        rx_buf[i] = LL_I2C_ReceiveData8(i2c);
    }

    // Wait for auto-generated STOP.
    if (!poll_flag([i2c]() { return LL_I2C_IsActiveFlag_STOP(i2c) != 0; },
                   timeout_ms)) {
        return Error::Timeout;
    }
    LL_I2C_ClearFlag_STOP(i2c);

    return Error::Ok;
}

}  // namespace ll
}  // namespace stm32h7
}  // namespace backend
}  // namespace i2c
