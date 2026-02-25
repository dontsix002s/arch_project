/**
 * @file  drivers/i2c/core/inc/i2c_traits.h
 * @brief i2c::Traits – the compile-time interface contract for I2C backends.
 *
 * Every backend (LL, HAL, CMSIS) must expose static methods that match the
 * signatures documented in this struct.  The Bus façade (i2c_bus.h) is
 * templated on a Backend type and delegates all calls to these statics.
 *
 * Extension point
 * ---------------
 * To add a new backend (e.g. for STM32L4 or SAMD21):
 *   1. Create a directory under drivers/i2c/backend/<family>/<type>/inc/.
 *   2. Define a struct that provides every static method listed in Traits.
 *   3. Instantiate i2c::Bus<YourBackend, YourPolicy> in your board config.
 *
 * Conformance check (C++17)
 * -------------------------
 * In C++17 conformance is verified implicitly: a compile error is emitted the
 * moment Bus<Backend,Policy> instantiates a method the backend does not
 * provide.  A C++20 concept is provided in a #ifdef block for richer
 * diagnostics when building with a C++20-capable compiler.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "drivers/i2c/core/inc/i2c_error.h"

namespace i2c {

/**
 * @brief Canonical interface that every I2C backend must satisfy.
 *
 * This struct is used as documentation and as a base for static_assert checks.
 * Backends do NOT inherit from Traits; they merely replicate its static API.
 */
struct Traits {
    // -------------------------------------------------------------------------
    // Lifecycle
    // -------------------------------------------------------------------------

    /// Initialise the I2C peripheral (clocks, GPIO, timing registers).
    /// Must be called once before any transfer.
    static void init();

    /// Release the peripheral: disable clocks, reset GPIO to Hi-Z.
    static void deinit();

    // -------------------------------------------------------------------------
    // Blocking transfers
    // -------------------------------------------------------------------------

    /// Write @p len bytes from @p buf to the device at @p addr.
    ///
    /// @param addr        7-bit or 10-bit slave address (right-aligned, no R/W bit).
    /// @param buf         Pointer to transmit data.
    /// @param len         Number of bytes to transmit (must be > 0).
    /// @param timeout_ms  Maximum time to wait, in milliseconds.
    /// @return            i2c::Error::Ok on success, error code otherwise.
    static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                       uint32_t timeout_ms);

    /// Read @p len bytes into @p buf from the device at @p addr.
    static Error read(uint16_t addr, uint8_t* buf, std::size_t len,
                      uint32_t timeout_ms);

    /// Combined write-then-read with a repeated-START between the two phases.
    ///
    /// Commonly used for register-addressed reads: write the register address,
    /// issue a repeated START, then read the register content.
    static Error write_read(uint16_t addr,
                            const uint8_t* tx_buf, std::size_t tx_len,
                            uint8_t*       rx_buf, std::size_t rx_len,
                            uint32_t       timeout_ms);
};

}  // namespace i2c
