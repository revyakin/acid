#ifndef PID_H
#define PID_H

#include <stdint.h>

#define SCAL_FACTOR 1024

#define P_TERM_MAX  (500 * SCAL_FACTOR)
#define I_TERM_MAX  (500 * SCAL_FACTOR)

typedef struct {

    /* PID koeff. */
    uint16_t    p_factor;
    uint16_t    i_factor;
    uint16_t    d_factor;

    /* Int. term summator */
    int32_t     sum_error;

    /* Last measured value. Use for diff. term */
    int16_t     last_meas;

    /* Max values */
    int32_t     max_sum;
    int32_t     max_error;
} pidc_t;


void    pid_init ( pidc_t *pid, uint16_t p_factor, uint16_t i_factor, uint16_t d_factor );
int16_t pid_controller ( pidc_t *pid, int16_t ref, int16_t meas );

#endif /* end of include guard: PID_H */
