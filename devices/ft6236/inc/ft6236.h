/**
 * @file  devices/ft6236/inc/ft6236.h
 * @brief FT6236 capacitive touch controller driver.
 *
 * The FT6236 is a single-point capacitive touch controller by FocalTech,
 * connected via I2C at address 0x38.  This driver sits on top of the generic
 * i2c::Bus façade and is therefore completely independent of the STM32H7
 * backend.
 *
 * Usage example
 * -------------
 * @code
 * #include "devices/ft6236/inc/ft6236.h"
 * #include "board/rev_a/inc/board_config.h"
 *
 * using Touch = devices::Ft6236<stmfw::board::rev_a::I2c1Bus>;
 *
 * Touch::init();
 *
 * devices::Ft6236<stmfw::board::rev_a::I2c1Bus>::TouchPoint tp{};
 * if (Touch::read_touch(tp) == i2c::Error::Ok && tp.touch_count > 0) {
 *     process_touch(tp.points[0].x, tp.points[0].y);
 * }
 * @endcode
 *
 * Extension point
 * ---------------
 * Replace the Bus template argument with any other i2c::Bus instantiation
 * (different backend, policy, or board) without modifying this file.
 *
 * @tparam Bus  An instantiation of i2c::Bus<Backend, Policy>.
 */
#pragma once

#include <cstdint>

#include "drivers/i2c/core/inc/i2c_error.h"

namespace devices {

/// Driver for the FT6236 single-/dual-point capacitive touch controller.
template <typename Bus>
struct Ft6236 {
    // -------------------------------------------------------------------------
    // I2C address
    // -------------------------------------------------------------------------

    /// Default 7-bit I2C slave address of the FT6236.
    static constexpr uint16_t kAddress = 0x38U;

    // -------------------------------------------------------------------------
    // Register map (selected registers)
    // -------------------------------------------------------------------------

    static constexpr uint8_t kRegDevMode    = 0x00U;  ///< Device mode
    static constexpr uint8_t kRegGestId     = 0x01U;  ///< Gesture ID
    static constexpr uint8_t kRegTouchCount = 0x02U;  ///< Number of touch points
    static constexpr uint8_t kRegTouch1Xh   = 0x03U;  ///< Touch 1 X MSB
    static constexpr uint8_t kRegTouch1Xl   = 0x04U;  ///< Touch 1 X LSB
    static constexpr uint8_t kRegTouch1Yh   = 0x05U;  ///< Touch 1 Y MSB
    static constexpr uint8_t kRegTouch1Yl   = 0x06U;  ///< Touch 1 Y LSB
    static constexpr uint8_t kRegFirmVer    = 0xA6U;  ///< Firmware version
    static constexpr uint8_t kRegChipId     = 0xA8U;  ///< Chip identifier (0x36 for FT6236)

    // -------------------------------------------------------------------------
    // Data structures
    // -------------------------------------------------------------------------

    /// Coordinates of a single detected touch point.
    struct Point {
        uint16_t x;  ///< X coordinate in pixels
        uint16_t y;  ///< Y coordinate in pixels
    };

    /// Result of one touch-panel read cycle.
    struct TouchPoint {
        uint8_t touch_count;   ///< Number of active touch points (0–2)
        Point   points[2];     ///< Touch point coordinates (valid for [0..touch_count-1])
    };

    // -------------------------------------------------------------------------
    // Driver API
    // -------------------------------------------------------------------------

    /// Initialise the FT6236: verify chip ID, apply default configuration.
    ///
    /// @param addr  I2C slave address (default: kAddress).
    /// @return      i2c::Error::Ok on success, error code otherwise.
    static i2c::Error init(uint16_t addr = kAddress);

    /// Read the current touch data from the FT6236.
    ///
    /// @param[out] out   Populated with touch count and point coordinates.
    /// @param      addr  I2C slave address (default: kAddress).
    /// @return           i2c::Error::Ok on success, error code otherwise.
    static i2c::Error read_touch(TouchPoint& out, uint16_t addr = kAddress);

    /// Read a single register byte.
    ///
    /// @param reg   Register address.
    /// @param[out] value  Register value.
    /// @param addr  I2C slave address.
    /// @return      i2c::Error::Ok on success.
    static i2c::Error read_reg(uint8_t reg, uint8_t& value,
                               uint16_t addr = kAddress);

    /// Write a single register byte.
    static i2c::Error write_reg(uint8_t reg, uint8_t value,
                                uint16_t addr = kAddress);
};

}  // namespace devices
