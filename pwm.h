#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define PWM_MAX_VALUE 2000
#define PWM_DEADTIME  0

void pwm_init(void);
void pwm_configure(uint16_t _delta, uint16_t _amplitude);

#endif /* end of include guard: PWM_H */
