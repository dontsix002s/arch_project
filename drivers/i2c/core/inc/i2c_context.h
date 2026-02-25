/**
 * @file  drivers/i2c/core/inc/i2c_context.h
 * @brief TransferContext – descriptor for a single I2C transaction.
 *
 * The Bus façade assembles a TransferContext from its call arguments and may
 * pass a pointer to it down to the backend for IRQ/DMA-driven modes where the
 * transfer outlives a single function call.
 *
 * In polling mode the context is typically stack-allocated and never escapes
 * the calling function.  In IRQ/DMA mode it must remain valid for the entire
 * duration of the transfer.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "drivers/i2c/core/inc/i2c_error.h"
#include "drivers/i2c/core/inc/i2c_types.h"

namespace i2c {

/// Descriptor for one complete I2C transfer request.
///
/// Backends operating in IRQ or DMA mode store a pointer to the active context
/// in their per-instance state and access it from the ISR to retrieve buffer
/// pointers and write back the result.
struct TransferContext {
    uint16_t       address;     ///< Slave address (7- or 10-bit, right-aligned)
    AddressMode    addr_mode;   ///< 7-bit or 10-bit addressing
    Direction      direction;   ///< First phase direction (Write or Read)

    const uint8_t* tx_buf;      ///< Transmit data (nullptr for a pure read)
    std::size_t    tx_len;      ///< Number of bytes to transmit

    uint8_t*       rx_buf;      ///< Receive buffer  (nullptr for a pure write)
    std::size_t    rx_len;      ///< Number of bytes to receive

    uint32_t       timeout_ms;  ///< Timeout (0 = no timeout)

    Error          result;      ///< Filled by the backend on completion
};

}  // namespace i2c
