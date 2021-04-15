#ifndef COMMON_FIFO_H
#define COMMON_FIFO_H

#include <sys/types.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid);

#endif