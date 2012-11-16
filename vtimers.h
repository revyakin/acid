#ifndef VTIMERS_H
#define VTIMERS_H

#define OPEN_LOOP_UPDATE_TIME 10

/* 
 * Add timers to this enum
 */
typedef enum {
    OPEN_LOOP_UPDATE_TIMER,
    
    VTIMERS_NUM /* Must be last element */
} vtimer_t;


/*
 * Function prototypes
 */
void         vtimers_update_state(void);
unsigned int vtimers_timer_elapsed(vtimer_t timer);
void         vtimers_set_timer(vtimer_t timer, unsigned int time);

#endif /* end of include guard: VTIMERS_H */
