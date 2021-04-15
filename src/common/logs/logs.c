#include "logs.h"
#include <stdio.h>
#include <time.h>

static const char *OPERATION_NAME[] = {"IWANT", "RECVD", "TSKEX",
                                       "TSKDN", "GOTRS", "2LATE",
                                       "CLOSD", "GAVUP", "FAILD"};

int log_operation(OPERATION operation, int request_id, int load, pid_t pid,
                  pthread_t tid, int answer) {
    if (load > 9 || load < 1) {
        return -1;
    }
    time_t inst = time(NULL);
    printf("%ld; %d; %d; %d; %ld; %d; %s\n", inst, request_id, load, pid, tid,
           answer, OPERATION_NAME[operation]);
    return 0;
}
