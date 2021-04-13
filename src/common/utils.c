#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

void make_fifo(){
    char buf[50];
    pid_t pid = getpid();
    pid_t tid = gettid();
    snprintf(buf + strlen(buf), sizeof(buf), "/temp/%d.%d", pid, tid);

};