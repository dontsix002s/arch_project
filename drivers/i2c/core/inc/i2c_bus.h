/**
 * @file  drivers/i2c/core/inc/i2c_bus.h
 * @brief i2c::Bus<Backend, Policy> – generic I2C bus façade.
 *
 * This header provides the single, portable entry point for all I2C
 * operations.  Application code and device drivers include only this header;
 * they never touch backend- or MCU-specific headers directly.
 *
 * Template parameters
 * -------------------
 * @tparam Backend  A concrete struct that satisfies the i2c::Traits interface
 *                  (see drivers/i2c/core/inc/i2c_traits.h).
 * @tparam Policy   A policy tag type that describes the transfer mode.
 *                  Provided tags: PollingPolicy, IrqPolicy, DmaIrqPolicy
 *                  (see drivers/i2c/policy/inc/).
 *
 * Usage example
 * -------------
 * @code
 * #include "drivers/i2c/core/inc/i2c_bus.h"
 * #include "drivers/i2c/backend/stm32h7/ll/inc/i2c_ll_backend.h"
 * #include "drivers/i2c/policy/inc/i2c_policy_polling.h"
 * #include "drivers/i2c/wait/inc/i2c_wait_baremetal.h"
 * #include "board/rev_a/inc/board_config.h"
 *
 * using MyBus = i2c::Bus<
 *     i2c::backend::stm32h7::ll::LlBackend<
 *         board::rev_a::kI2c1Periph,
 *         i2c::policy::PollingPolicy,
 *         i2c::wait::BaremetalWait>,
 *     i2c::policy::PollingPolicy>;
 *
 * MyBus::init();
 * uint8_t cmd = 0x01;
 * auto err = MyBus::write(0x3C, &cmd, 1, 100);
 * @endcode
 *
 * Extension point
 * ---------------
 * Swap the Backend template argument to change the underlying driver without
 * touching device-driver or application code.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "drivers/i2c/core/inc/i2c_error.h"

namespace i2c {

    /// Generic I2C bus façade.
    ///
    /// All public methods forward directly to Backend's static methods.
    /// Policy is carried as a tag so the backend can select the correct code path
    /// at compile time (e.g. via if constexpr or tag dispatch).
	template <typename Backend, typename Policy>
    struct Bus {
	    
	    
	    /// Initialise the underlying I2C peripheral.
        static void init() { Backend::init(); }

        /// Release the underlying I2C peripheral.
        static void deinit() { Backend::deinit(); }

        /// Write @p len bytes from @p buf to the device at @p addr.
        static Error write(uint16_t addr, const uint8_t* buf, std::size_t len,
                       uint32_t timeout_ms = 100U) {
            return Backend::write(addr, buf, len, timeout_ms);
        }

        /// Read @p len bytes into @p buf from the device at @p addr.
        static Error read(uint16_t addr, uint8_t* buf, std::size_t len,
                      uint32_t timeout_ms = 100U) {
            return Backend::read(addr, buf, len, timeout_ms);
        }

        /// Write then read with a repeated-START in between.
        static Error write_read(uint16_t addr,
                            const uint8_t* tx_buf, std::size_t tx_len,
                            uint8_t*       rx_buf, std::size_t rx_len,
                            uint32_t       timeout_ms = 100U) {
            return Backend::write_read(addr, tx_buf, tx_len, rx_buf, rx_len,
                                   timeout_ms);
        }
	    
	    
	    static void on_event_isr() { Backend::on_event_isr(); }
	    static void on_error_isr() { Backend::on_error_isr(); }

	    // optional for DMA later:
	    //static void on_dma_tx_isr() { Backend::on_dma_tx_isr(); }
	    //static void on_dma_rx_isr() { Backend::on_dma_rx_isr(); }	    
    };

}
