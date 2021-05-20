#include <stdio.h>
#include <stdlib.h>

#include "server/message_queue/message_queue.h"
#include "common/log/log.h"
#include "server/parser/parser.h"

int main(int argc, char **argv){
     if(!valid_server_options(argc, argv)){
        fprintf(stderr, "Usage: %s <-t nsecs> [-l bufsz] <fifoname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int nsecs = atoi(argv[2]);
    char *fifoname = argv[argc - 1];
    int bufsz;
    if(argc == 6) bufsz = atoi(argv[4]);

    printf("nsecs = %d, fifoname = %s, bufsz = %d\n", nsecs, fifoname, bufsz);

    /*MessageQueue* queue = init_message_queue();
    Message msg1, msg2;
    assemble_message(&msg1, 1, 1, 4);
    assemble_message(&msg2, 2, 5, 7);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    printf("queue size: %u\n", message_queue_size(queue));
    message_queue_push(queue, &msg1);
    message_queue_push(queue, &msg2);
    printf("queue size: %u\n", message_queue_size(queue));
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    Message msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    msg1.rid = 30; // must not affect the queue
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    message_queue_pop(queue);
    msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    printf("queue size: %u\n", message_queue_size(queue));
    message_queue_pop(queue);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    destroy_message_queue(queue);*/

    return 0;
}