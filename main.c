#include <stm32f10x.h>

#include "pwm.h"
#include "sine.h"
#include "drive.h"
#include "state_machine.h"

#define STK_LOAD_VAL 3000 - 1

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

    __enable_irq();
}

int main(void)
{
    init();

    for(;;)
    {
        state_machine();
    }

    return 0;
}

