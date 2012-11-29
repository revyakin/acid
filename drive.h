#ifndef DRIVE_H
#define DRIVE_H

struct _open_loop_params_t {
    int frequency;
    int acceleration;
};

typedef struct _open_loop_params_t open_loop_params_t;

void drive_halt(void);
void drive_open_loop(open_loop_params_t *params);
void drive_close_loop(int ref_speed);
int open_loop_acceleration(int reference, int acceleration);

#endif /* end of include guard: DRIVE_H */
