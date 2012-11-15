#ifndef SINE_H
#define SINE_H

#include <stm32f10x.h>

#define SINE_MAX_FREQUENCY 546
#define SINE_MAX_AMPLITUDE ((PWM_MAX / 2) - 1)

struct _sine_param_t {
    u8  direction;      /* 1 or -1 */
    u16 freq_m;         /* sine freq: F = (freq_m * Fclk) / 65536 */
    u16 amplitude_pwm;  /* amplitude from 0 to (PWM_MAX_VALUE / 2) */
};

typedef struct _sine_param_t sine_param_t;

/*
 * Function prototypes
 */
void sine_reset(void);
void sine_set_params(sine_param_t *sine_param);

/*
 *  Must be called in PWM update interrupt
 */
void sine_generation_task(void);


#endif /* end of include guard: SINE_H */

