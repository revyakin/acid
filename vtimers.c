#include "vtimers.h"

static unsigned int vtimers[VTIMERS_NUM];

void vtimers_update_state(void)
{
    int i;
    for (i = 0; i < VTIMERS_NUM; i++)
        if (vtimers[i] > 0)
            vtimers[i] --;
}

unsigned int vtimers_timer_elapsed(vtimer_t timer)
{
    return !(vtimers[timer] > 0);
}

void vtimers_set_timer(vtimer_t timer, unsigned int time)
{
    vtimers[timer] = time;
}

