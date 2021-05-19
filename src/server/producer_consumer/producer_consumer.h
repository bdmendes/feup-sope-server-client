#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include "../../common/message/message.h"

int init_producer_consumer(unsigned buffer_size);

int push_pending_request(const Message *message);

void *producer(void *arg);

void *consumer(void *arg);

void destroy_producer_consumer();

#endif