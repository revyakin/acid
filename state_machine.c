#include "state_machine.h"
#include "vtimers.h"
#include "drive.h"

#define FL_SIZE 6

typedef enum {
    SM_STATE_RUN
} state_t;

state_t sm_state;

open_loop_params_t open_loop_params = {
    .frequency    = 273,
    .acceleration = 1
};

int fl_index = 0;
int frequency_list[FL_SIZE] = {
    273,
    410,
    0,
    -273,
    -410,
    410
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

    if (vtimers_timer_elapsed(CHANGE_SPEED_TIMER)) {
        vtimers_set_timer(CHANGE_SPEED_TIMER, CHANGE_SPEED_TIME);

        open_loop_params.frequency = frequency_list[fl_index];

        if (++fl_index > FL_SIZE) {
            fl_index = 0;
        }
    }
}
