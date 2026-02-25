/**
 * @file  drivers/i2c/core/inc/i2c_types.h
 * @brief Common I2C enumeration types shared across all layers.
 *
 * This header is intentionally dependency-free (only <cstdint>) so it can be
 * included by every layer – core, policy, wait, backend, board, and devices –
 * without creating circular dependencies.
 */
#pragma once

#include <cstdint>

namespace i2c {

/// Addressing mode for an I2C transaction.
enum class AddressMode : uint8_t {
    Bits7  = 0,  ///< 7-bit slave address (most common)
    Bits10 = 1,  ///< 10-bit slave address (rare, large address spaces)
};

/// Bus clock speed preset.
enum class Speed : uint32_t {
    Standard = 100'000U,    ///< 100 kHz – Standard Mode (SM)
    Fast     = 400'000U,    ///< 400 kHz – Fast Mode (FM)
    FastPlus = 1'000'000U,  ///< 1 MHz   – Fast Mode Plus (FM+)
};

/// Transfer direction.
enum class Direction : uint8_t {
    Write = 0,  ///< Master → Slave
    Read  = 1,  ///< Slave  → Master
};

}  // namespace i2c
