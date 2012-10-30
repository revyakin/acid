#include <stm32f10x.h>
#include <pwm.h>

int main(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    GPIOC->CRH &= ~GPIO_CRH_CNF8;
    GPIOC->CRH |= GPIO_CRH_MODE8;

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 15);
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    __enable_irq();

    pwm_init();
    pwm_configure(273, 1000);

    for(;;)
        GPIOC->ODR &= ~GPIO_ODR_ODR8;

    return 0;
}

