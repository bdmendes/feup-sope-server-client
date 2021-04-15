#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include "fifo.h"

int make_private_fifo(char name[]) {
    if (mkfifo(name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("could not make fifo");
        return -1;
    }
    return 0;
}

int delete_private_fifo(char name[]) {
    if (remove(name) == -1) {
        perror("could not delete fifo");
        return -1;
    }
    return 0;
}