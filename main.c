#include <stm32f10x.h>
#include "pwm.h"
#include "drive.h"
#include "usart.h"

#define STK_LOAD_VAL 300000 - 1

s16             freq        = 273;
s16             old_freq    = 0;
sine_param_t    sinep;
u32 counter;

void init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;

    GPIOC->CRH &= ~GPIO_CRH_CNF9;
    GPIOC->CRH |= GPIO_CRH_MODE9;

    pwm_init();

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    SysTick->LOAD = STK_LOAD_VAL;
    SysTick->CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;

    usart_init();
}

int main(void)
{
    init();
    __enable_irq();

    for(;;)
    {
        if (GPIOC->ODR & GPIO_ODR_ODR8)
            GPIOC->ODR &= ~GPIO_ODR_ODR8;

        asm("":::"memory");

        if (old_freq != freq)
        {
            old_freq = freq;

            sinep.amplitude_pwm = drive_vf_control(freq);
            sinep.freq_m        = freq;

            pwm_reconfigure(&sinep);
        }

        if (usart_getch() != USART_NO_DATA) {
            GPIOC->ODR ^= GPIO_ODR_ODR9;
            tx_dma_start_transfer("Hello!\r\n", 8);
        }

    }

    return 0;
}
