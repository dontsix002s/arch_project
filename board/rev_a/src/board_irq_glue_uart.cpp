//-----------------------------------------------------------------------------
// board_irq_glue_uart.cpp
//-----------------------------------------------------------------------------

// -- Includes ------------------------------------
#include "board/inc/board_revision_select.h"


extern "C" void USART1_IRQHandler(void)
{
	board::active::uart1::on_irq_isr();
}
