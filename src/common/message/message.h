#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>

typedef struct {
    int rid;       // request id
    pid_t pid;     // process id
    pthread_t tid; // thread id
    int tskload;   // task load
    int tskres;    // task result
} Message;

typedef enum {IWANT, RECVD, TSKEX, TSKDN, GOTRS, TOOLATE, CLOSD, GAVUP, FAILD} OPERATION;

void assemble_message(Message *message, int request_id, int load, int answer);

void log_operation(OPERATION operation, int request_id, int load, int answer);

#endif