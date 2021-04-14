#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

void get_private_fifo_name(char buf[], int pid, int tid);

int communication(char buf[], int i, int t, int pid, int tid, int res, bool logs);

#endif