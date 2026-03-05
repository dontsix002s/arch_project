#pragma once

#include "stm32h7xx.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_usart.h"

#include "mcu/stm32h7/inc/detail/stm32h7_ll_helpers.h"
#include "drivers/uart/backend/stm32h7/low_layer/inc/uart_ll_backend.h"

namespace uart::backend::stm32h7::ll {

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    void LlBackend<Mcu, Policy>::bind(const uart::backend::stm32h7::BoardCfg& cfg)
    {
        s_cfg = &cfg;
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    const uart::backend::stm32h7::BoardCfg& LlBackend<Mcu, Policy>::cfg()
    {
        // ћожно добавить assert, но оставим минимально
        return *s_cfg;
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    static USART_TypeDef* usart()
    {
        return reinterpret_cast<USART_TypeDef*>(Mcu.usart_base);
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    void LlBackend<Mcu, Policy>::enable_usart_clock()
    {
        switch (Mcu.instance_index)
        {
        case 0U: LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); break;
        default: break;
        }
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    void LlBackend<Mcu, Policy>::init()
    {
        mcu::stm32h7::ll::enable_gpio_clock(cfg().tx.port_index);
        mcu::stm32h7::ll::enable_gpio_clock(cfg().rx.port_index);
        enable_usart_clock();

        LL_GPIO_InitTypeDef gpio{};
        gpio.Mode = LL_GPIO_MODE_ALTERNATE;
        gpio.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        gpio.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        gpio.Pull = LL_GPIO_PULL_UP;

        gpio.Pin = mcu::stm32h7::ll::pin_mask(cfg().tx.pin);
        gpio.Alternate = mcu::stm32h7::ll::ll_gpio_af(cfg().tx.af);
        LL_GPIO_Init(mcu::stm32h7::ll::gpio_from_port_index(cfg().tx.port_index), &gpio);

        gpio.Pin = mcu::stm32h7::ll::pin_mask(cfg().rx.pin);
        gpio.Alternate = mcu::stm32h7::ll::ll_gpio_af(cfg().rx.af);
        LL_GPIO_Init(mcu::stm32h7::ll::gpio_from_port_index(cfg().rx.port_index), &gpio);

        LL_USART_Disable(usart<Mcu, Policy>());
        LL_USART_SetTransferDirection(usart<Mcu, Policy>(), LL_USART_DIRECTION_TX_RX);
        LL_USART_ConfigCharacter(usart<Mcu, Policy>(), LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
        LL_USART_SetOverSampling(usart<Mcu, Policy>(), LL_USART_OVERSAMPLING_16);
        //LL_USART_SetBaudRate(usart<Mcu, Policy>(), cfg().clock_hz, LL_USART_OVERSAMPLING_16, cfg().baud);
        LL_USART_Enable(usart<Mcu, Policy>());
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    void LlBackend<Mcu, Policy>::deinit()
    {
        LL_USART_Disable(usart<Mcu, Policy>());
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    uart::Error LlBackend<Mcu, Policy>::write(const uint8_t* buf, std::size_t len, uint32_t timeout_ms)
    {
        (void)timeout_ms;
        for (std::size_t i = 0; i < len; ++i)
        {
            while (!LL_USART_IsActiveFlag_TXE(usart<Mcu, Policy>())) {}
            LL_USART_TransmitData8(usart<Mcu, Policy>(), buf[i]);
        }
        while (!LL_USART_IsActiveFlag_TC(usart<Mcu, Policy>())) {}
        return uart::Error::Ok;
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    uart::Error LlBackend<Mcu, Policy>::read(uint8_t* buf, std::size_t len, uint32_t timeout_ms)
    {
        (void)timeout_ms;
        for (std::size_t i = 0; i < len; ++i)
        {
            while (!LL_USART_IsActiveFlag_RXNE(usart<Mcu, Policy>())) {}
            buf[i] = LL_USART_ReceiveData8(usart<Mcu, Policy>());
        }
        return uart::Error::Ok;
    }

    template <const uart::backend::stm32h7::McuPeriph& Mcu, typename Policy>
    void LlBackend<Mcu, Policy>::on_irq_isr()
    {
        // TODO
    }

} // namespace uart::backend::stm32h7::ll