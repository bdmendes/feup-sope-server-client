#include "message.h"

int assemble_message(Message *message, const int request_id, const int load,
                     const pid_t pid, const pthread_t tid, const int answer) {

    if (load > 9 || load < 1)
        return -1;

    message->pid = pid;
    message->rid = request_id;
    message->tid = tid;
    message->tskload = load;
    message->tskres = answer;

    return 0;
}