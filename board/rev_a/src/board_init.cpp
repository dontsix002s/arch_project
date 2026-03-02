/**
 * @file  board/rev_a/src/board_init.cpp
 * @brief Revision-A board initialisation.
 *
 * Implements board::rev_a::init().  Initialisation order:
 *   1. Publish reset-default clock snapshot so that SystemTimer has a valid
 *      frequency before the hardware timer starts.
 *   2. Start SystemTimer on reset clocks so that timeouts work during PLL init.
 *   3. Apply the Performance clock profile (configures HSE + PLL1).
 *   4. Refresh SystemTimer so it runs at the correct rate for the new clocks.
 */

#include "board/rev_a/inc/board_rev.h"
#include "board/rev_a/inc/board_clock.h"
#include "system/inc/system_clocks.h"
#include "system/inc/system_timer.h"

namespace board::rev_a
{
	///------------------------------------------------------------------------
	/// init()
	///------------------------------------------------------------------------
    void init()
    {
	    // Что сюда может/должно входить - всё, что зависит от разводки, 
	    // выбранной частотной конфигурации и какая периферия реально подключена	    
		// Обычно сюда входит:
		//   - clock tree “профиль платы” PLL / делители
		//   - выбор конкретного системного таймера (NVIC priority, prescaler)
	    //   - GPIO + pinmux настройка выводов под I2C / SPI / UART, CS, RESET, IRQ линии тачскрина и т.д.
	    //   - включение клоков периферии GPIOxEN, I2CxEN, SPIxEN и т.п.(или подготовка,
	    //     если делаете это в драйверах — но хотя бы “что вообще доступно на плате” решается тут)
	    //   - init устройств платы(опционально) например, включить питание дисплея, reset внешних микросхем,
	    //     поднять backlight но “высокоуровневое”(инициализация приложения) лучше оставить app,
	    //     чтобы board не разрастался 
	    // Основной критерий: если завтра вы переносите проект на другую плату(rev_b) — меняется в основном тут
		
		
        // 1) Publish the reset-default clock snapshot (64 MHz HSI)
        stmfw::system::clocks::set(stmfw::system::clocks::reset_defaults());

        // 2) Start the system timebase on reset clocks so the BSP can use
        //    board::rev_a::time::now_ms() during the PLL sequence.
        stmfw::system::SystemTimer<SystemTimerTraits>::init();

		// 3) Configure RCC / PLL for maximum performance.
        board::rev_a::clock::apply(clock::Profile::Balanced);

        // 4) Reconfigure the timebase prescaler for the new APB1 timer clock.
        stmfw::system::SystemTimer<SystemTimerTraits>::refresh();
	    
    }
}
