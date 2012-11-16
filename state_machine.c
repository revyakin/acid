#include "state_machine.h"
#include "drive.h"

int sm_state;
int sampling_time_flag = 0;

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

    if (sampling_time_flag) {
        sampling_time_flag = 0;

        drive_open_loop(&open_loop_params);
    }
}
