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

#define FL_SIZE 6

typedef enum {
    SM_STATE_RUN
} state_t;

state_t sm_state;

open_loop_params_t open_loop_params = {
    .frequency    = 273,
    .acceleration = 1
};

#define CONTROL_RUN 1

uint16_t CONTROL = 0;
uint16_t STATUS  = 0;

void state_machine(void)
{
    /* Update state
     */
    MB_WRITE_REG (MB_REG_STATUS, STATUS);
    CONTROL = MB_READ_REG_U (MB_REG_CONTROL);

    switch(sm_state) {

        default:
            break;
    }

    if ( !(CONTROL & CONTROL_RUN) ) {
        sine_reset();        
    }

    if ( (CONTROL & CONTROL_RUN) && vtimers_timer_elapsed(OPEN_LOOP_UPDATE_TIMER) ) {
        vtimers_set_timer(OPEN_LOOP_UPDATE_TIMER, OPEN_LOOP_UPDATE_TIME);

        drive_close_loop( open_loop_params.frequency );

    }

    modbus_fsm();
}

