#include "client/fifo/fifo.h"
#include "client/input_validation/input_validation.h"
#include "common/logs/logs.h"
#include "common/utils/utils.h"
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void *func_1(void *a) {
    OPERATION ot = IWANT;
    log_operation(ot, 4, 5, getpid(), pthread_self(), 6);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (!valid_client_options(argc, argv)) {
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