#pragma once
#include <cstddef>
#include <cstdint>

#include "drivers/uart/core/inc/uart_error.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_mcu_periph.h"
#include "drivers/uart/backend/stm32h7/common/inc/uart_stm32h7_board_cfg.h"

namespace uart::backend::stm32h7::ll {

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    struct LlBackend
    {
        static void bind(const uart::backend::stm32h7::BoardCfg& cfg);

        static void init();
        static void deinit();

        static uart::Error write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms);
        static uart::Error read(uint8_t* buf, std::size_t len, uint32_t timeout_ms);

        static void on_irq_isr();

    private:
        static void enable_usart_clock();
        static const uart::backend::stm32h7::BoardCfg& cfg();
        static inline const uart::backend::stm32h7::BoardCfg* s_cfg = nullptr;
    };

} // namespace uart::backend::stm32h7::ll