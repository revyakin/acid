#include <stm32f10x.h>

#include "pwm.h"
#include "sine.h"
#include "drive.h"

#define STK_LOAD_VAL 30000 - 1

sine_param_t sinep;

void init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    SysTick->LOAD = STK_LOAD_VAL;
    SysTick->CTRL |= SysTick_CTRL_TICKINT | SysTick_CTRL_ENABLE;

    pwm_init();
    sine_reset();

    pwm_output_enable();
}

u8  sampling_time_flag = 0;

static unsigned int abs(int x)
{
    return (x < 0) ? (unsigned int) (-x) : (unsigned int) x;
}

int open_loop_acceleration(int reference, int acceleration)
{
    static int current;

    if (current == reference)
        return current;
    
    if (abs(current - reference) < acceleration) {
        current = reference;
        return current;
    }

    if (current > reference)
    {
        current -= acceleration;
    } else {
        current += acceleration;
    }

    return current;
}

int ref_speed    = 512;
int acceleration = 1;

int main(void)
{
    init();
    __enable_irq();

    for(;;)
    {
        if (GPIOC->ODR & GPIO_ODR_ODR8)
            GPIOC->ODR &= ~GPIO_ODR_ODR8;

        asm("":::"memory");

        if (sampling_time_flag)
        {
            sampling_time_flag = 0;

            int frequency = open_loop_acceleration(ref_speed, acceleration);

            sinep.amplitude_pwm = drive_vf_control(frequency);
            sinep.freq_m        = frequency;

            sine_set_params(&sinep);
        }
    }

    return 0;
}

