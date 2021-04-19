#include "message.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void assemble_message(Message *message, int request_id, int load, int answer) {
    message->pid = getpid();
    message->tid = pthread_self();
    message->rid = request_id;
    message->tskload = load;
    message->tskres = answer;
}
