#ifndef REQUEST_H
#define REQUEST_H

#include <time.h>
#include <stdbool.h>

typedef struct {
    int load;
    int rid;
    struct timespec private_fifo_timeout;
    int public_fifo_fd;
} Request;

bool is_server_closed();

void *request_server(void *arg);

#endif