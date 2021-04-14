#include "utils.h"
#include <dirent.h>
#include <stdio.h>

void get_private_fifo_name(char buf[], int pid, int tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%d", pid, tid);
}

void communication(char buf[], int i, int t, int pid, int tid, int res){
    snprintf(buf, PATH_MAX, "%d ; %d ; %d ; %d ; %d", i, t, pid, tid, res);
}
