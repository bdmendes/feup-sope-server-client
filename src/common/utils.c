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
    
    sprintf(buf, "/tmp/%d.%d", pid, tid);
    if(mkfifo(buf, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
        perror("Couldn't make fifo");
        return -1;
    }
    return 0;
}

int delete_fifo(){
    char buf[20];
    pid_t pid = getpid();
    pid_t tid = gettid();
    sprintf(buf, "/tmp/%d.%d", pid, tid);
    if(unlink(buf) < 0) {
        perror("Couldn't delete fifo");
        return -1;
    }
    return 0;
}