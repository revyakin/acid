#include "pwm.h"
#include "sin.h"
#include "timer1.h"

#include <stm32f10x.h>

#define PHASE_U_OFFSET 0
#define PHASE_V_OFFSET 21845
#define PHASE_W_OFFSET 43690
#define PWM_ZERO       (PWM_MAX_VALUE / 2)

uint16_t phase;
int16_t  delta;
uint16_t amplitude;

void pwm_init()
{
    phase = 0;
    delta = 0;
    amplitude = 0;

    timer1_init();
}

void pwm_reconfigure(sine_param_t *sine_param)
{
    // TODO: this must be atomic

    /* amplitude saturation */
    if (sine_param->amplitude_pwm > ((PWM_MAX_VALUE / 2) - 1))
    {
        amplitude = (PWM_MAX_VALUE / 2) - 1;
    } else {
        amplitude = sine_param->amplitude_pwm;
    }

    delta = sine_param->freq_m;
}

void timer1_update_interrupt()
{
    /* 
     * clear interrupt flag
     */
    timer1_clear_if(); 

    GPIOC->ODR |= GPIO_ODR_ODR8;

    uint8_t  index;
    uint16_t pwm;

    /* phase increment */
    phase += delta;
 
    /* phase U duty cycle */
    index = (phase + PHASE_U_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;

    timer1_u_load(PWM_ZERO + pwm);

    /* phase V duty cycle */
    index = (phase + PHASE_V_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;

    timer1_v_load(PWM_ZERO + pwm);

    /* phase W duty cycle */
    index = (phase + PHASE_W_OFFSET) >> 8;
    pwm = (sine_table[index] * amplitude) >> 7;

    timer1_w_load(PWM_ZERO + pwm);
}

