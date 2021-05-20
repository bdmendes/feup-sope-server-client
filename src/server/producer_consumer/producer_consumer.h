#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include "../../common/message/message.h"

/**
 * Default buffer size
 */
#define DEFAULT_BUF_SIZE 100

/**
 * @brief Function to step ut the producer consumer
 * @param buffer_size Size of the buffer of messages
 * @return 0 if successful, -1 otherwise 
 */
int pc_init(unsigned buffer_size);

/**
 * @brief Function to add a new request to the producer consumer
 * @param message Message to be added 
 */
void pc_push_pending_request(const Message *message);

/**
 * @brief Function to inform that the server is already closed
 */
void pc_signal_server_closed();

/**
 * @brief Function to free the resources of producer consumer
 */
void pc_destroy();

#endif