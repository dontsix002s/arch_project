//---------------------------------
//
//-----------------------------------------------------------

#include "board/inc/board_revision_select.h"


extern "C" void I2C1_EV_IRQHandler(void)
{
	board::active::i2c1::on_event_isr();
}

extern "C" void I2C1_ER_IRQHandler(void)
{
	board::active::i2c1::on_error_isr();
}
