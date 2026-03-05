#include "board/rev_a/inc/board_config.h"
#include "drivers/uart/backend/stm32h7/low_layer/inc/uart_ll_backend_impl.h"

template struct uart::backend::stm32h7::ll::LlBackend<
    board::rev_a::uart1::Mcu,
    board::rev_a::uart1::Policy
>;