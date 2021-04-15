#include "utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void get_private_fifo_name(char buf[], int pid, int tid) {
    snprintf(buf, PATH_MAX, "/tmp/%d.%d", pid, tid);
}

bool is_all_digits(char *string) {
    size_t size = strlen(string);
    while (size--) {
        if (!isdigit(string[size])) {
            return false;
        }
    }
    return true;
}
