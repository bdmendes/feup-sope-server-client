#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%ld", pid, tid);
}

int assemble_i_to_res(char buf[], int i, int t, pid_t pid, pthread_t tid, int res, bool logs){
    if(t > 9 || t < 1)
        return -1;
    if (logs)
        snprintf(buf, PATH_MAX, "%d ; %d ; %d ; %ld ; %d", i, t, pid, tid, res);
    else
        snprintf(buf, PATH_MAX, "%d %d %d %ld %d", i, t, pid, tid, res);
    return 0;
}
