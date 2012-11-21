#ifndef READLINE_H
#define READLINE_H

#define READLINE_BUF_SIZE 25
#define READLINE_MAX_ARGS 10

void readline_init(void);
void readline_receive_task(void);

/*
 * Extern function
 */
void readline_exec(int argc, char ** argv);

#endif /* end of include guard: READLINE_H */
