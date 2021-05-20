#ifndef PRODUCER_CONSUMER_H
#define PRODUCER_CONSUMER_H

#include "../../common/message/message.h"

#define DEFAULT_BUF_SIZE 100

int pc_init(unsigned buffer_size);

void pc_push_pending_request(const Message *message);

void pc_signal_server_closed();

void pc_destroy();

#endif