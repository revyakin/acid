#ifndef TIMER1_H
#define TIMER1_H

#include <stm32f10x.h>

#define PWM_MAX_VALUE 2000

#define timer1_u_load(x) (TIM1->CCR1 = (x))
#define timer1_v_load(x) (TIM1->CCR2 = (x))
#define timer1_w_load(x) (TIM1->CCR3 = (x))

#define timer1_output_enable()  TIM1->BDTR |= TIM_BDTR_MOE
#define timer1_output_disable() TIM1->BDTR &= ~TIM_BDTR_MOE

#define timer1_update_interrupt TIM1_UP_TIM16_IRQHandler
#define timer1_clear_if() TIM1->SR &= ~TIM_SR_UIF

void timer1_init(void);

#endif /* end of include guard: TIMER1_H */
