#ifndef REQUEST_H
#define REQUEST_H

#include <time.h>
#include <stdbool.h>


/**
 * @brief Struct which encapsulates the request attributes.
 */
typedef struct {
    /*
    * task load
    */
    int load;
    /*
    * Request ID
    */
    int rid;
    /*
    * Time until the thread give up waiting a response.
    */
    struct timespec private_fifo_timeout;
    /*
    * Public fifo's file descriptor.
    */
    int public_fifo_fd;
} Request;

/**
 * @brief Checks if server is closed.
 * 
 * @return true if it is closed, false otherwise
 */
bool is_server_closed();

/**
 * @brief Send a request to the server and read its response, logging all the relevant operations. 
 * 
 * @param arg information to be send in the request.
 */
void *request_server(void *arg);

#endif