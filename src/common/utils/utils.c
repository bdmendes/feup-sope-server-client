#include "utils.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>

void get_private_fifo_name(char buf[], pid_t pid, pthread_t tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%ld", pid, tid);
}

int assemble_operation_status(char buf[], int request_id, int load, pid_t pid,
                              pthread_t tid, int answer, bool comma) {
    if (load > 9 || load < 1)
        return -1;
    if (comma)
        snprintf(buf, PATH_MAX, "%d ; %d ; %d ; %ld ; %d", request_id, load,
                 pid, tid, answer);
    else
        snprintf(buf, PATH_MAX, "%d %d %d %ld %d", request_id, load, pid, tid,
                 answer);
    return 0;
}
