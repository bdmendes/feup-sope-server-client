#include "message_queue.h"
#include <stdlib.h>
#include <stdio.h>

struct MessageQueueNode {
    Message element;
    struct MessageQueueNode* next;
};

struct MessageQueue {
    struct MessageQueueNode* head;
};

MessageQueue* init_message_queue(){
    MessageQueue* queue = malloc(sizeof(struct MessageQueue));
    if (queue == NULL){
        fprintf(stderr, "Could not allocate message queue\n");
        return NULL;
    }
    queue->head = NULL;
    return queue;
}

int message_queue_push(MessageQueue* queue, const Message* msg){
    if (queue == NULL){
        fprintf(stderr, "Message queue is null\n");
        return -1;
    }
    struct MessageQueueNode* node = (struct MessageQueueNode*)
        malloc(sizeof(struct MessageQueueNode));
    if (node == NULL){
        fprintf(stderr, "Could not allocate message queue node\n");
        return -1;
    }
    node->element = *msg;
    node->next = NULL;
    if (queue->head == NULL){
        queue->head = node;
    } else {
        queue->head->next = node;
    }
    return 0;
}

int message_queue_pop(MessageQueue* queue){
    if (queue->head == NULL){
        fprintf(stderr, "Message queue is empty\n");
        return -1;
    }
    free(queue->head);
    queue->head = queue->head->next;
    return 0;
}

Message message_queue_front(const MessageQueue* queue){
    return queue->head->element;
}

bool message_queue_empty(const MessageQueue* queue){
    return queue->head == NULL;
}

void destroy_message_queue(MessageQueue* queue){
    while(!message_queue_empty(queue)){
        message_queue_pop(queue);
    }
    free(queue);
}
