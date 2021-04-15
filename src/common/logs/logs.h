#ifndef LOGS_H
#define LOGS_H

#include <pthread.h>

typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

int log_operation(OPERATION operation, int request_id, int load, pid_t pid,
                  pthread_t tid, int answer);

#endif