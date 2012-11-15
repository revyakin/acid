#ifndef PWM_H
#define PWM_H

#include <stm32f10x.h>

#define PWM_MAX     2000
#define PWM_ZERO    (PWM_MAX / 2)

#define pwm_load_u(x) TIM1->CCR1 = (x)
#define pwm_load_v(x) TIM1->CCR2 = (x)
#define pwm_load_w(x) TIM1->CCR3 = (x)

void pwm_init();

void pwm_output_disable();
void pwm_output_enable();

#endif /* end of include guard: PWM_H */
