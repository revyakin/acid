
#include "pid.h"

void pid_init( pidc_t *pid, uint16_t p_factor, uint16_t i_factor, uint16_t d_factor )
{
    /* Reset PID */
    pid->last_meas = 0;
    pid->sum_error = 0;

    /* Tuning */
    pid->p_factor = p_factor;
    pid->i_factor = i_factor;
    pid->d_factor = d_factor;

    /* Calculate max values */
    pid->max_error = P_TERM_MAX / (pid->p_factor + 1);
    pid->max_sum   = I_TERM_MAX / (pid->i_factor + 1);
}

int16_t pid_controller( pidc_t *pid, int16_t ref, int16_t meas )
{
    int32_t p_term, i_term, d_term;
    int32_t error, temp;

    error = ref - meas;

    if ( error > pid->max_error ) {
        p_term = P_TERM_MAX;
    } else if ( error < -pid->max_error ) {
        p_term = -P_TERM_MAX;
    } else {
        p_term = pid->p_factor * error;
    }

    temp = pid->sum_error + error;
    if ( temp > pid->max_sum ) {
        i_term = I_TERM_MAX;
    } else if ( temp < -I_TERM_MAX ) {
        i_term = -I_TERM_MAX;
    } else {
        pid->sum_error = temp;
        i_term = pid->i_factor * pid->sum_error;
    }

    d_term = pid->d_factor * pid->last_meas - meas;
    pid->last_meas = meas;

    return (int16_t) ((p_term + i_term + d_term) / SCAL_FACTOR);
}

