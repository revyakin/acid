#include "state_machine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vtimers.h"
#include "drive.h"
#include "encoder.h"
#include "readline.h"
#include "usart.h"
#include "sine.h"
#include "pid.h"
#include "modbus.h"

typedef enum {
    SM_STATE_INIT = 0,
    SM_STATE_STOP,
    SM_STATE_RUN,
    SM_STATE_BREAKING
} state_t;

state_t sm_state = SM_STATE_STOP;

//open_loop_params_t open_loop_params = {
//    .frequency    = 273,
//    .acceleration = 1
//};

#define CONTROL_RUN 1

uint16_t status  = 0;
uint16_t control = 0;

extern int16_t enc_delta;

int16_t  speed_meas = 0;
int16_t  speed_ref  = 0;
int16_t  pid_output = 0;
int16_t  k_fb       = 0;

pidc_t    pid;

void state_machine(void)
{
    /* Update state
     */
    control = MB_READ_REG_U(MB_REG_CONTROL);
    speed_ref = MB_READ_REG_S(MB_REG_SPEED_REF);



    switch(sm_state) {

        case SM_STATE_INIT:

            pid_init( &pid,
                      MB_READ_REG_S(MB_REG_PID_PROP),
                      MB_READ_REG_S(MB_REG_PID_INT),
                      MB_READ_REG_S(MB_REG_PID_DIF) );

            k_fb = MB_READ_REG_S(MB_REG_KFB);

            sm_state = SM_STATE_RUN;

            break;

        case SM_STATE_STOP:

            if ( control & CONTROL_RUN ) {
                sm_state = SM_STATE_INIT;
                break;
            }   

            break;
            
        case SM_STATE_RUN:

            if ( !(control & CONTROL_RUN) ) {
                sine_reset();
                sm_state = SM_STATE_BREAKING;
            }

            if (vtimers_timer_elapsed(OPEN_LOOP_UPDATE_TIMER) ) {
                vtimers_set_timer(OPEN_LOOP_UPDATE_TIMER, OPEN_LOOP_UPDATE_TIME);

                speed_meas = (uint16_t) (((int32_t) (enc_delta * k_fb)) / 1024);

                pid_output = pid_controller( &pid, speed_ref, speed_meas );

                sine_param_t sine_params;
                sine_params.direction     = is_negative(pid_output);
                sine_params.amplitude_pwm = vf_control(abs(pid_output));
                sine_params.freq_m        = abs(pid_output);

                sine_set_params(&sine_params);
            }

            break;

        case SM_STATE_BREAKING:

            if (enc_delta == 0) {
                sm_state = SM_STATE_STOP;
                break;
            }

            break;

        default:
            break;
    }

    MB_WRITE_REG(MB_REG_SPEED_MEAS, speed_meas);
    MB_WRITE_REG(MB_REG_PID_OUTPUT, pid_output);
    MB_WRITE_REG(MB_REG_STATUS,     (uint16_t) sm_state);


    modbus_fsm();
}

