#include "utils.h"
#include <dirent.h>
#include <stdio.h>

void get_private_fifo_name(char buf[], int pid, int tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%d", pid, tid);
}
