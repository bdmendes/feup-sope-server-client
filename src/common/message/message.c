#include "message.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static const char *OPERATION_NAME[] = {"IWANT", "RECVD", "TSKEX",
                                       "TSKDN", "GOTRS", "2LATE",
                                       "CLOSD", "GAVUP", "FAILD"};

void assemble_message(Message *message, int request_id, int load, int answer) {
    message->pid = getpid();
    message->tid = pthread_self();
    message->rid = request_id;
    message->tskload = load;
    message->tskres = answer;
}

void log_operation(OPERATION operation, int request_id, int load, int answer) {
    time_t inst = time(NULL);
    printf("%ld; %d; %d; %d; %ld; %d; %s\n", inst, request_id, load, getpid(),
           pthread_self(), answer, OPERATION_NAME[operation]);
}
