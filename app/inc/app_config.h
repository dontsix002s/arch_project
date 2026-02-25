/**
 * @file  app/inc/app_config.h
 * @brief Application-level compile-time configuration.
 *
 * Centralises all application tunables (timeouts, task stack sizes, feature
 * flags, …) in one place so they can be overridden per build without modifying
 * source files.
 */
#pragma once

#include <cstdint>

namespace app {

/// Default I2C transfer timeout in milliseconds.
static constexpr uint32_t kI2cTimeoutMs = 100U;

/// Touch polling interval in milliseconds (used in the super-loop).
static constexpr uint32_t kTouchPollMs = 20U;

}  // namespace app
