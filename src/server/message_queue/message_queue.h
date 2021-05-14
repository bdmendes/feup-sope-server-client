#ifndef SERVER_MESSAGE_QUEUE_H
#define SERVER_MESSAGE_QUEUE_H

#include <stdbool.h>
#include "../../common/message/message.h"

typedef struct MessageQueue MessageQueue;

MessageQueue* init_message_queue();

int message_queue_push(MessageQueue* queue, const Message* msg);

int message_queue_pop(MessageQueue* queue);

Message message_queue_front(const MessageQueue* queue);

unsigned message_queue_size(const MessageQueue* queue);

bool message_queue_empty(const MessageQueue* queue);

void destroy_message_queue(MessageQueue* queue);

#endif