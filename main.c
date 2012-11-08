#include <stm32f10x.h>
#include "pwm.h"
#include "drive.h"

s16 freq = 273;
s16 old_freq = 0;
sine_param_t sinep;

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    __enable_irq();

    pwm_init();

    sinep.amplitude_pwm = 700;
    sinep.freq_m        = -546;

    pwm_reconfigure(&sinep);

    for(;;)
    {
        GPIOC->ODR &= ~GPIO_ODR_ODR8;

        asm("":::"memory");

        if (old_freq != freq)
        {
            old_freq = freq;

            sinep.amplitude_pwm = drive_vf_control(freq);
            sinep.freq_m        = freq;

            pwm_reconfigure(&sinep);
        }
    }

    return 0;
}

