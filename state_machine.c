#include "state_machine.h"
#include "vtimers.h"
#include "drive.h"

typedef enum {
    SM_STATE_RUN
} state_t;

state_t sm_state;

open_loop_params_t open_loop_params = {
    .frequency    = 273,
    .acceleration = 1
};

void state_machine(void)
{
    switch(sm_state) {

        default:
            break;
    }

    if (vtimers_timer_elapsed(OPEN_LOOP_UPDATE_TIMER)) {
        vtimers_set_timer(OPEN_LOOP_UPDATE_TIMER, OPEN_LOOP_UPDATE_TIME);

        drive_open_loop(&open_loop_params);
    }
}
