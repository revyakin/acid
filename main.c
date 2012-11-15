#include <stm32f10x.h>
#include "pwm.h"
#include "drive.h"
#include "usart.h"

#define STK_LOAD_VAL 30000 - 1

sine_param_t    sinep;

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
}

s16 current_freq = 0;
u16 accel        = 1;
s16 ref_freq     = 410;
u8  ts_flag      = 0;


void acceleration(void)
{
    if (current_freq == ref_freq)
        return;

    if (current_freq > ref_freq)
    {
        // Deceleration
        if ((current_freq - ref_freq) < accel)
            current_freq = ref_freq;
        else
            current_freq -= accel;
    } else {
        // Acceleration
        if ((ref_freq - current_freq) < accel)
            current_freq = ref_freq;
        else
            current_freq += accel;
    }
}

int main(void)
{
    int ind = 0;
    u16 timer = 0;
    s16 list[4] = {273, 546, 273, 0};

    init();
    __enable_irq();

    for(;;)
    {
        asm("":::"memory");

        if (GPIOC->ODR & GPIO_ODR_ODR8)
            GPIOC->ODR &= ~GPIO_ODR_ODR8;

        if (ts_flag)
        {
            ts_flag = 0;

            acceleration();

            sinep.amplitude_pwm = drive_vf_control(current_freq);
            sinep.freq_m        = current_freq;

            pwm_reconfigure(&sinep);

            if (++timer > 1500)
            {
                timer = 0;

                GPIOC->ODR ^= GPIO_ODR_ODR9;

                ref_freq = list[ind];
                ind++;
                ind &= 0x3;
            }
        }

    }

    return 0;
}

void SysTick_Handler()
{
    ts_flag = 1;
}
