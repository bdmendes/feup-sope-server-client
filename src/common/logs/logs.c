#include "logs.h"
#include "../utils/utils.h"
#include <dirent.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static const char *OPERATION_NAME[] = {"IWANT", "RECVD", "TSKEX",
                                       "TSKDN", "GOTRS", "2LATE",
                                       "CLOSD", "GAVUP", "FAILD"};

int log_operation(OPERATION operation, int request_id, int load, pid_t pid,
                  pthread_t tid, int answer) {
    time_t inst = time(NULL);
    char buf[PATH_MAX];

    if (assemble_operation_status(buf, request_id, load, pid, tid, answer,
                                  true) != 0)
        return -1;

    printf("%ld ; %s ; %s\n", inst, buf, OPERATION_NAME[operation]);
    return 0;
};
