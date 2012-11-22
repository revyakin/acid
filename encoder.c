#include "encoder.h"
#include <stm32f10x.h>

#define TIM2_RELOAD_VAL 65536

int tim2_old_cnt = 0;

static void gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~GPIO_CRL_MODE0;
    GPIOA->CRL &= ~GPIO_CRL_MODE1;

    GPIOA->CRL &= ~GPIO_CRL_CNF0; 
    GPIOA->CRL &= ~GPIO_CRL_CNF1; 

    GPIOA->CRL |= GPIO_CRL_CNF0_1; 
    GPIOA->CRL |= GPIO_CRL_CNF1_1; 

    //GPIOA->ODR |= GPIO_ODR_ODR0;
    //GPIOA->ODR |= GPIO_ODR_ODR1;
}

static void timer2_encoder_init()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->ARR = TIM2_RELOAD_VAL - 1;

    TIM2->SMCR  |= TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1;
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;
    TIM2->CCMR1 |= TIM_CCMR1_CC2S_0;

    TIM2->CR1 |= TIM_CR1_CEN;
}

void encoder_init(void)
{
    gpio_init();
    timer2_encoder_init();

    tim2_old_cnt = 0;
}

int encoder_get_speed(void)
{
    int speed;
    int cnt       = TIM2->CNT;
    int direction = TIM2->CR1 & TIM_CR1_DIR;

    if ( direction ) {
        
        if (tim2_old_cnt >= cnt) {
            speed = tim2_old_cnt - cnt;
        } else {
            speed = TIM2_RELOAD_VAL - (cnt - tim2_old_cnt);
        }

    } else {

        if (cnt >= tim2_old_cnt) {
            speed = cnt - tim2_old_cnt;
        } else {
            speed = TIM2_RELOAD_VAL - (tim2_old_cnt - cnt);
        }

    }

    tim2_old_cnt = cnt;

    return ( direction ) ? -speed : speed;
}

