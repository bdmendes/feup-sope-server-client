#ifndef COMMON_FIFO_H
#define COMMON_FIFO_H

#include <sys/types.h>

/**
 * @brief Function to get the name of the private fifo accoording to pid and tid
 * @param buf Buffer to store the name into
 * @param pid pid of the client process
 * @param tid Posix id of the client thread
 */
void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid);

#endif