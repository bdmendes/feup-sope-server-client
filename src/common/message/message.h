#ifndef MESSAGE_H
#define MESSAGE_H

#include <sys/types.h>

typedef struct
{
    int rid;       // request id
    pid_t pid;     // process id
    pthread_t tid; // thread id
    int tskload;   // task load
    int tskres;    // task result
} Message;

int assemble_message(Message *message, const int request_id, const pid_t pid, const pthread_t tid, int load, int answer);

#endif