#include "fifo.h"
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void get_private_fifo_name(char buf[]) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%ld", getpid(), pthread_self());
}