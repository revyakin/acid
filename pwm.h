#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "timer1.h"

#define MAX_AMPLITUDE  (PWM_MAX_VALUE / 2)

struct _sine_param_t {
    u16 freq_m;         /* sine freq: F = (freq_m * Fclk) / 65536 */
    s16 amplitude_pwm;  /* amplitude from 0 to (PWM_MAX_VALUE / 2) */
};

typedef struct _sine_param_t sine_param_t;

/*
 * Function prototypes
 */
void pwm_init(void);
void pwm_reconfigure(sine_param_t *sine_param);

#endif /* end of include guard: PWM_H */
