/**
 * @file  drivers/i2c/core/inc/i2c_error.h
 * @brief I2C error-code enumeration.
 *
 * All layers (core, backend, device drivers) return or inspect values of
 * i2c::Error.  No other layer-specific headers are included here.
 */
#pragma once

#include <cstdint>

namespace i2c {

/// Outcome of an I2C operation.
enum class Error : uint8_t {
    Ok              = 0,  ///< Transfer completed successfully
    BusError        = 1,  ///< Misplaced START or STOP condition detected
    ArbitrationLost = 2,  ///< Bus arbitration lost (multi-master topology)
    NackAddress     = 3,  ///< NACK received after the address phase
    NackData        = 4,  ///< NACK received during the data phase
    Timeout         = 5,  ///< Operation did not complete within the timeout
    Busy            = 6,  ///< Bus or peripheral is already in use
    InvalidArg      = 7,  ///< Caller supplied a null pointer or zero length
};

}  // namespace i2c
