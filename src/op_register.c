#include "op_register.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

int op_reg(int i, int t, OPERATION op, int res){
    time_t inst = time(NULL);
    if(t > 9 || t < 1)
        return -1;
    pid_t pid = getpid();
    pid_t tid = gettid();
    char buf[6];
    switch (op) {
        case IWANT:
            sprintf(buf, "IWANT");
            break;
        case RECVD:
            sprintf(buf, "RECVD");
            break;
        case TSKEX:
            sprintf(buf, "TSKEX");
            break;
        case TSKDN:
            sprintf(buf, "TSKDN");
            break;
        case GOTRS:
            sprintf(buf, "GOTRS");
            break;
        case LATE2:
            sprintf(buf, "2LATE");
            break;
        case CLOSD:
            sprintf(buf, "CLOSD");
            break;
        case GAVUP: 
            sprintf(buf, "GAVUP");
            break;
        case FAILD:
            sprintf(buf, "FAILD");
            break;
        default: 
            return -1;
    }
    printf("%ld ; %d ; %d ; %d ; %d ; %d ; %s\n", inst, i, t, pid, tid, res, buf);
    return 0;
};


