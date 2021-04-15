#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <sys/types.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid);

int assemble_i_to_res(char buf[], int i, int t, pid_t pid, pthread_t tid, int res, bool logs);

#endif