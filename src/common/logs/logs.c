#include "logs.h"
#include "../utils/utils.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h> 
#include <pthread.h>
#include <dirent.h>

static const char* OPERATION_NAME[] = { "IWANT", "RECVD", "TSKEX", "TSKDN", "GOTRS", "2LATE", "CLOSD", "GAVUP", "FAILD" };

int op_reg(int i, int t, OPERATION op, int res){
    time_t inst = time(NULL);
    pid_t pid = getpid();
    pthread_t tid = pthread_self();
    
    char buf[PATH_MAX];

    if(communication(buf, i, t, pid, tid, res, true)!= 0)
        return -1;

    printf("%ld ; %s ; %s\n", inst, buf, OPERATION_NAME[op]);
    return 0;
};


