#include "fifo.h"
#include <dirent.h>
#include <stdio.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%ld", pid, tid);
}