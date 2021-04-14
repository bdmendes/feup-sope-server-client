#ifndef UTILS_H
#define UTILS_H

void get_private_fifo_name(char buf[], int pid, int tid);

void communication(char buf[], int i, int t, int pid, int tid, int res);

#endif