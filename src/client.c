#include "client/fifo/fifo.h"
#include "common/utils.h"
#include <stdio.h>

#include <dirent.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *func_1(void *a) {
    char fifo_name[PATH_MAX];
    get_private_fifo_name(fifo_name, getpid(), pthread_self());
    make_private_fifo(fifo_name);
    delete_private_fifo(fifo_name);
    make_private_fifo(fifo_name);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    int opt = getopt(argc, argv, "t:");

    if (opt != 't' || optind != 3 || argc != 4 || !is_all_digits(optarg)) {
        fprintf(stderr, "Usage: %s <-t nsecs> <fifoname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nsecs = atoi(optarg);
    char fifoname[PATH_MAX];
    strcpy(fifoname, argv[optind]);

    printf("%d seconds, fifo %s\n", nsecs, fifoname);

    pthread_t id1;
    if (pthread_create(&id1, NULL, func_1, NULL) != 0)
        exit(-1);
    pthread_join(id1, NULL);
    return 0;
}