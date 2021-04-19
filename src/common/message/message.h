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

void assemble_message(Message *message, int request_id, int load, int answer);

#endif