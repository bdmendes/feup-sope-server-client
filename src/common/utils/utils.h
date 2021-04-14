#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

void get_private_fifo_name(char buf[], int pid, long unsigned int tid);

int assemble_i_to_res(char buf[], int i, int t, int pid, long unsigned int tid, int res, bool logs);

#endif