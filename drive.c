#include "drive.h"
#include "sine.h"
#include "pwm.h"
#include "pid.h"
#include "encoder.h"

#define VF_SLOPE         938
#define VF_MAX_AMPLITUDE 1000
#define VF_MIN_AMPLITUDE 100

/*static*/ unsigned int abs(int x)
{
    return (x < 0) ? (unsigned int) (-x) : (unsigned int) x;
}

/*static*/ unsigned int is_negative(int x)
{
    return (x < 0) ? 1 : 0;
}

/*static*/ unsigned int vf_control(int frequency)
{
    unsigned int amplitude;

    frequency = abs(frequency);

    /* Vf law */
    amplitude = frequency * VF_SLOPE / 256;

    /* Amplitude saturation */
    if (amplitude > VF_MAX_AMPLITUDE)
    {
        amplitude = VF_MAX_AMPLITUDE;
    } 
    else if (amplitude < VF_MIN_AMPLITUDE)
    {
        amplitude = VF_MIN_AMPLITUDE;
    }

    return amplitude;
}

static int open_loop_acceleration(int reference, int acceleration)
{
    static int current;

    if (current == reference)
        return current;
    
    if (abs(current - reference) < acceleration) {
        current = reference;
        return current;
    }

    if (current > reference)
    {
        current -= acceleration;
    } else {
        current += acceleration;
    }

    return current;
}

void drive_open_loop(open_loop_params_t *params)
{
    int ref_frequency = params->frequency;
    int acceleration  = params->acceleration;

    int frequency = open_loop_acceleration(ref_frequency, acceleration);

    sine_param_t sine_params;
    sine_params.direction     = is_negative(frequency);
    sine_params.amplitude_pwm = vf_control(abs(frequency));
    sine_params.freq_m        = abs(frequency);

    sine_set_params(&sine_params);
}

void drive_halt(void)
{
    pwm_output_disable();
    sine_reset();
}

void drive_close_loop(int ref_speed)
{
    int measure = encoder_get_speed();

    int pid_output = pid_controller(ref_speed, measure);

    sine_param_t sine_params;
    sine_params.direction     = is_negative(pid_output);
    sine_params.amplitude_pwm = vf_control(abs(pid_output));
    sine_params.freq_m        = abs(pid_output);

    sine_set_params(&sine_params);
}


