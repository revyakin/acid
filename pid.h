#ifndef PID_H
#define PID_H

#define MAX_P_TERM 65535
#define MAX_I_TERM 10000000
#define SCALING_FACTOR 1024


void pid_init(int pFactor, int iFactor, int dFactor);
int pid_controller(int reference, int measure);

#endif /* end of include guard: PID_H */
