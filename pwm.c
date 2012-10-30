#include "pwm.h"
#include <stm32f10x.h>
#include "sin.h"

void static timer1_gpio_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPAEN;

    GPIOA->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9   | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_CNF8_1 | GPIO_CRH_CNF9_1 | GPIO_CRH_CNF10_1;
    GPIOA->CRH |= GPIO_CRH_MODE8  | GPIO_CRH_MODE9  | GPIO_CRH_MODE10;
    
    GPIOB->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_CNF14   | GPIO_CRH_CNF15);
    GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF14_1 | GPIO_CRH_CNF15_1;
    GPIOB->CRH |= GPIO_CRH_MODE13  | GPIO_CRH_MODE14  | GPIO_CRH_MODE15;
}

void pwm_init()
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    /* set timer counter to 0 */
    TIM1->CNT = 0x0000;

    /* set timer prescaller to 1 */
    TIM1->PSC = 0x0000;

    /* enable preload for autoreload register */
    TIM1->CR1 |= TIM_CR1_ARPE;

    /* set autoreload register */
    TIM1->ARR = PWM_MAX_VALUE;

    /* enable count */
    TIM1->CR1 |= TIM_CR1_CEN;

    /* generate update event to update counter,
     * direction and autoreload register */
    TIM1->EGR |= TIM_EGR_UG;

    /* set pwm1 mode (OCxM = 110) and enable preload (OCxPE = 1)
     * for output/compare channel 1 and channel 2 */
    TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 |
        TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 |
        TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

    /* set pwm1 mode (OCxM = 110) and enable preload (OCxPE = 1)
     * for output/compare channel 3 */
    TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1 |
        TIM_CCMR2_OC3PE;

    /* enable output/compare outputs (CCxE = 1)
     * and complementary outputs (CCxNE = 1) */
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E |
        TIM_CCER_CC3E | TIM_CCER_CC1NE | TIM_CCER_CC2NE |
        TIM_CCER_CC3NE;

    /* set initial pwm values */
    TIM1->CCR1 = 0x0000;
    TIM1->CCR2 = 0x0000;
    TIM1->CCR3 = 0x0000;

    /* main output enable */
    // TODO: OSSI and OSSR bits
    TIM1->BDTR |= TIM_BDTR_MOE;

    /* enable UPDATE interrupt request */
    TIM1->DIER |= TIM_DIER_UIE;

    timer1_gpio_init();
}

#define PHASE_U_OFFSET 0
#define PHASE_V_OFFSET 21845
#define PHASE_W_OFFSET 43690
#define PWM_ZERO 1000

volatile uint16_t phase = 0;
volatile uint16_t delta = 0;
volatile uint16_t amplitude;

void pwm_configure(uint16_t _delta, uint16_t _amplitude)
{
    delta = _delta;
    amplitude = _amplitude;
}

void TIM1_UP_TIM16_IRQHandler()
{
    TIM1->SR &= ~TIM_SR_UIF;

    GPIOC->ODR |= GPIO_ODR_ODR8;

    uint8_t  index;
    uint16_t pwm;

    /* phase increment */
    phase += delta;
 
    /* phase U duty cycle */
    index = (phase + PHASE_U_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;
    TIM1->CCR1 = PWM_ZERO + pwm;

    /* phase V duty cycle */
    index = (phase + PHASE_V_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;
    TIM1->CCR2 = PWM_ZERO + pwm;

    /* phase W duty cycle */
    index = (phase + PHASE_W_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;
    TIM1->CCR3 = PWM_ZERO + pwm;
}

