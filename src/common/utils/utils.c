#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%ld", pid, tid);
}

int assemble_i_to_res(char buf[], int request_id, int load, pid_t pid, pthread_t tid, int answer, bool logs){
    if(load > 9 || load < 1)
        return -1;
    if (logs)
        snprintf(buf, PATH_MAX, "%d ; %d ; %d ; %ld ; %d", request_id, load, pid, tid, answer);
    else
        snprintf(buf, PATH_MAX, "%d %d %d %ld %d", request_id, load, pid, tid, answer);
    return 0;
}
