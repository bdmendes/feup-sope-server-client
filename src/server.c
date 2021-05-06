#include <stdio.h>
#include "server/message_queue/message_queue.h"
#include "common/log/log.h"

int main(){
    MessageQueue* queue = init_message_queue();
    Message msg1, msg2;
    assemble_message(&msg1, 1, 1, 4);
    assemble_message(&msg2, 2, 5, 7);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    message_queue_push(queue, &msg1);
    message_queue_push(queue, &msg2);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    Message msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    msg1.rid = 30; // must not affect the queue
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    message_queue_pop(queue);
    msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    message_queue_pop(queue);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    destroy_message_queue(queue);
}