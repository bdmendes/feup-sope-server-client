#include "log.h"
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static const char *OPERATION_NAME[] = {"IWANT", "RECVD", "TSKEX",
                                       "TSKDN", "GOTRS", "2LATE",
                                       "CLOSD", "GAVUP", "FAILD"};

void log_operation(OPERATION operation, int request_id, int load, int answer) {
    time_t inst = time(NULL);
    printf("%ld; %d; %d; %d; %ld; %d; %s\n", inst, request_id, load, getpid(),
           pthread_self(), answer, OPERATION_NAME[operation]);
}
