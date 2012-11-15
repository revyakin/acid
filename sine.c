#include "sine.h"
#include <stm32f10x.h>

#include "sine_table.h"
#include "pwm.h"

#define PHASE_SHIFT_U   (65536 / 360 * 0  )
#define PHASE_SHIFT_V   (65536 / 360 * 120)
#define PHASE_SHIFT_W   (65536 / 360 * 240)

static u16 m_phase;
static s16 m_delta;
static u16 m_amplitude;

void sine_reset()
{
    /* Set initial values */
    m_phase     = 0;
    m_delta     = 0;
    m_amplitude = 0;
}

void sine_set_params(sine_param_t *sine_param)
{
    /* Direction management */
    s8  direction = (sine_param->direction) ? -1 : 1;

    /* Sine amplitude */
    u16 amplitude = sine_param->amplitude_pwm;

    /* Sine frequency */
    u16 frequency = sine_param->freq_m;

    /* Amplitude saturation */
    if (amplitude > SINE_MAX_AMPLITUDE) {
        amplitude = SINE_MAX_AMPLITUDE;
    }

    /* Frequency saturation */
    if (frequency > SINE_MAX_FREQUENCY) {
        frequency = SINE_MAX_FREQUENCY;
    }

    __disable_irq();

    /* Begin atomic block */

    m_delta     = frequency * direction;
    m_amplitude = amplitude;

    /* End atomic block */

    __enable_irq();
}

void sine_generation_task(void)
{
    uint8_t  index;
    uint16_t pwm;

    /* phase increment */
    m_phase += m_delta;
 
    /* phase U duty cycle */
    index = (m_phase + PHASE_SHIFT_U) >> 8;
    pwm = (sine_table[index] * m_amplitude) >> 7;

    pwm_load_u(PWM_ZERO + pwm);

    /* phase V duty cycle */
    index = (m_phase + PHASE_SHIFT_V) >> 8;
    pwm = (sine_table[index] * m_amplitude) >> 7;

    pwm_load_v(PWM_ZERO + pwm);

    /* phase W duty cycle */
    index = (m_phase + PHASE_SHIFT_W) >> 8;
    pwm = (sine_table[index] * m_amplitude) >> 7;

    pwm_load_w(PWM_ZERO + pwm);
}

