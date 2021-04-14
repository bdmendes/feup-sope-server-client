#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "fifo.h"

int make_private_fifo(char name[]) {
    if (mkfifo(name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Couldn't make fifo");
        return -1;
    }
    return 0;
}

int delete_private_fifo(char name[]) {
    if (remove(name) == -1) {
        perror("Couldn't delete fifo");
        return -1;
    }
    return 0;
}