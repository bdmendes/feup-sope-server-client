#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../common/fifo/fifo.h"
#include "../../common/log/log.h"
#include "../../common/message/message.h"
#include "request.h"

static volatile bool server_closed = false;

bool is_server_closed() {
    return server_closed;
}

/**
 * @brief Free the request
 *
 * @param request to be freed
 */
static void thread_free_request(void *request) {
    free(request);
}

/**
 * @brief Unlink fifo.
 *
 * @param fifo_name fifo's name.
 */
static void thread_unlink_fifo(void *fifo_name) {
    char *fifo_name_ptr = (char *)fifo_name;
    if (unlink(fifo_name_ptr) == -1) {
        perror("thread unlink");
    }
}

/**
 * @brief Close fifo.
 *
 * @param fd fifo file descriptor.
 */
static void thread_close_fifo(void *fd) {
    int fifo_fd = *((int *)fd);
    if (close(fifo_fd) == -1) {
        perror("thread close");
    }
}

void *request_server(void *arg) {
    /* Assemble message to send */
    Message sent_msg;
    Request *request = (Request *)arg;
    assemble_message(&sent_msg, request->rid, request->load, -1);
    pthread_cleanup_push(thread_free_request, arg);

    /* Make private fifo */
    char private_fifo_name[PATH_MAX];
    get_private_fifo_name(private_fifo_name, getpid(), pthread_self());
    if (mkfifo(private_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Could not make private fifo");
        pthread_exit(NULL);
    }
    pthread_cleanup_push(thread_unlink_fifo, private_fifo_name);

    /* Open private fifo for reading */
    int private_fifo_fd = open(private_fifo_name, O_RDONLY | O_NONBLOCK);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        pthread_exit(NULL);
    }
    pthread_cleanup_push(thread_close_fifo, &private_fifo_fd);

    /* Write request to public fifo */
    if (write(request->public_fifo_fd, &sent_msg, sizeof(sent_msg)) == -1) {
        perror("Could not write to public fifo");
        pthread_exit(NULL);
    }
    log_operation(IWANT, request->rid, request->load, -1);

    /* Read server response from private fifo */
    Message received_msg;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(private_fifo_fd, &set);
    int ready_fds = pselect(private_fifo_fd + 1, &set, NULL, NULL,
                            &request->private_fifo_timeout, NULL);
    if (ready_fds == -1) {
        perror("Could not wait for private fifo read");
    } else if (ready_fds == 0) {
        log_operation(GAVUP, request->rid, request->load, -1);
    } else {
        if (read(private_fifo_fd, &received_msg, sizeof(Message)) !=
            sizeof(Message)) {
            perror("Could not read message from private fifo");
        } else {
            OPERATION operation;
            if (received_msg.tskres == -1) {
                server_closed = true;
                operation = CLOSD;
            } else {
                operation = GOTRS;
            }
            log_operation(operation, request->rid, request->load,
                          received_msg.tskres);
        }
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);
    pthread_exit(NULL);
}
