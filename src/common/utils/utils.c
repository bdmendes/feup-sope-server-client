#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

void get_private_fifo_name(char buf[], int pid, int tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%d", pid, tid);
}

int communication(char buf[], int i, int t, int pid, int tid, int res, bool logs){
    if(t > 9 || t < 1)
        return -1;
    if (logs)
        snprintf(buf, PATH_MAX, "%d ; %d ; %d ; %d ; %d", i, t, pid, tid, res);
    else
        snprintf(buf, PATH_MAX, "%d %d %d %d %d", i, t, pid, tid, res);
    return 0;
}
