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

#define FL_SIZE 6

typedef enum {
    SM_STATE_RUN
} state_t;

state_t sm_state;

open_loop_params_t open_loop_params = {
    .frequency    = 273,
    .acceleration = 1
};

int speed;

int fl_index = 0;
int frequency_list[FL_SIZE] = {
    273,
    410,
    0,
    -273,
    -410,
    410
};

int run = 0;

void state_machine(void)
{
    switch(sm_state) {

        default:
            break;
    }

    if (!run) {
        sine_reset();        
    }

    if (run && vtimers_timer_elapsed(OPEN_LOOP_UPDATE_TIMER)) {
        vtimers_set_timer(OPEN_LOOP_UPDATE_TIMER, OPEN_LOOP_UPDATE_TIME);

        drive_close_loop( open_loop_params.frequency );

//        speed = encoder_get_speed();
//        drive_open_loop(&open_loop_params);
    }

//    if (vtimers_timer_elapsed(CHANGE_SPEED_TIMER) && run_auto) {
//        vtimers_set_timer(CHANGE_SPEED_TIMER, CHANGE_SPEED_TIME);
//
//        open_loop_params.frequency = frequency_list[fl_index];
//
//        if (++fl_index > FL_SIZE) {
//            fl_index = 0;
//        }
//    }

    readline_receive_task();
}

int kp = 0;
int ki = 0;
int kd = 0;

void readline_exec(int argc, char *argv[])
{
    if ( !strcmp(argv[0], "set") || !strcmp(argv[0], "get") ) {

        //char buf[16];
        int set = (argv[0][0] == 's');
        
        if (argc < 3)
            goto few_args;

        if ( !strcmp(argv[1], "freq") ) {
            
            if (set)
                open_loop_params.frequency = atoi(argv[2]);
            
        }

        else if ( !strcmp(argv[1], "accel") ) {

            if (set)
                open_loop_params.acceleration = atoi(argv[2]);

        }

        else if ( !strcmp(argv[1], "kp") ) {
            kp = atoi(argv[2]);
        }

        else if ( !strcmp(argv[1], "ki") ) {
            ki = atoi(argv[2]);
        }

        else if ( !strcmp(argv[1], "kd") ) {
            kd = atoi(argv[2]);
        }

        else {
            goto unknown_param;
        }

    } 

    else if ( !strcmp(argv[0], "run") ) {
        run = 1;
        GPIOC->ODR |= GPIO_ODR_ODR9;
    }

    else if ( !strcmp(argv[0], "stop") ) {
        run = 0;
        GPIOC->ODR &= ~GPIO_ODR_ODR9;
    }

    else if ( !strcmp(argv[0], "pid-reset") ) {
        pid_init(kp, ki, kd);
    }

    else {
        usart_puts("Unknown command.\r\n");
    }

    return;

few_args:
    usart_puts("Too few arguments.\r\n");
    return;

unknown_param:
    usart_puts("Unknown parameter.\r\n");
    return;
}

