#include "utils.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

int make_fifo(){
    char buf[20];
    pid_t pid = getpid();
    pid_t tid = gettid();
    
    snprintf(buf + strlen(buf), sizeof(buf), "/tmp/%d.%d", pid, tid);
    if(mkfifo(buf, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
        perror("Couldn't make fifo");
        return -1;
    }
    return 0;
};