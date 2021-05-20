#ifndef SERVER_MESSAGE_QUEUE_H
#define SERVER_MESSAGE_QUEUE_H

#include <stdbool.h>
#include "../../common/message/message.h"

/**
 * @brief Queue created to save the messages 
 */
typedef struct MessageQueue MessageQueue;

/**
 * @brief Function to allocate a message queue
 * @return Pointer to the queue
 */
MessageQueue* init_message_queue();

/**
 * @brief Function to insert a new message to a queue
 * @param queue Queue to receive the message
 * @param msg Message to insert
 * @return 0 uppon success, -1 otherwise
 */
int message_queue_push(MessageQueue* queue, const Message* msg);

/**
 * @brief Function to remove a message from a queue
 * @param queue Queue where the message will be removed
 * @return 0 uppon success, -1 otherwise
 */
int message_queue_pop(MessageQueue* queue);


/**
 * @brief Function to return the front message in a queue
 * @param queue Queue where the message will be 
 * @return Message in the front
 */
Message message_queue_front(const MessageQueue* queue);

/**
 * @brief Function to checks the size of a queue
 * @param queue The queue to evaluate
 * @return The size of the queue
 */
unsigned message_queue_size(const MessageQueue* queue);

/**
 * @brief Function to checks if the queue is empty
 * @param queue The queue to evaluate
 * @return True if the queue is empty, false otherwise
 */
bool message_queue_empty(const MessageQueue* queue);

/**
 * @brief Function to free the queue resources
 * @param queue Queue to delete
 */
void destroy_message_queue(MessageQueue* queue);

#endif