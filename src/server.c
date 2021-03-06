#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/log/log.h"
#include "common/timer/timer.h"
#include "server/message_queue/message_queue.h"
#include "server/parser/parser.h"
#include "server/producer_consumer/producer_consumer.h"

/**
 * @brief Function that receives the messages and creates the producers to treat them
 * @param fifo_name Name of the public fifo
 * @return 0 if no errors occurred, -1 otherwise 
 */
int listener(char fifo_name[]) {
    /* Open public fifo */
    int public_fifo_fd = open(fifo_name, O_RDONLY | O_NONBLOCK);
    if (public_fifo_fd == -1) {
        fprintf(stderr, "Could not open fifo\n");
        return -1;
    }

    while (true) {
        /* Assemble remaining time */
        struct timespec remaining_time;
        get_timer_remaining_time(&remaining_time);
        if (time_is_up(&remaining_time)) {
            pc_signal_server_closed();
        }

        /* Wait for public fifo with relaxed timeout to attend late requests */
        remaining_time.tv_sec += 1;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(public_fifo_fd, &set);
        int ready_fds = pselect(public_fifo_fd + 1, &set, NULL, NULL,
                                &remaining_time, NULL);
        if (ready_fds == -1) {
            fprintf(stderr, "Wait for public fifo failed\n");
            usleep(BUSY_WAIT_DELAY_MICROS);
            continue;
        } else if (ready_fds == 0) {
            break;
        }

        /* Read request and dispatch it */
        Message message;
        if (read(public_fifo_fd, &message, sizeof(Message)) !=
            sizeof(Message)) {
            get_timer_remaining_time(&remaining_time);
            if (time_is_up(&remaining_time)) {
                break;
            } else {
                continue;
            }
        }
        log_operation(RECVD, message.rid, message.tskload, message.tskres);
        pc_push_pending_request(&message);
    }

    /* Close public fifo */
    if (close(public_fifo_fd) == -1) {
        perror("Could not close public fifo");
        return -1;
    }

    return 0;
}

/**
 * @brief Function to free resources
 */
void cleanup() {
    pc_destroy();
    destroy_timer();
}

int main(int argc, char **argv) {
    if (!valid_server_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> [-l bufsz] <fifoname>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    setbuf(stdout, NULL);
    int nsecs = atoi(argv[2]);
    char *public_fifo_name = argv[argc - 1];
    int buf_size = argc == 6 ? atoi(argv[4]) : DEFAULT_BUF_SIZE;

    if (mkfifo(public_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create public fifo '%s'\n",
                    public_fifo_name);
            exit(EXIT_FAILURE);
        }
    }

    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    if (pc_init(buf_size) != 0) {
        fprintf(stderr, "Could not init producer consumer\n");
        destroy_timer();
        exit(EXIT_FAILURE);
    }

    atexit(cleanup);

    if (listener(public_fifo_name) == -1) {
        fprintf(stderr, "Listener failed\n");
    }

    if (unlink(public_fifo_name) == -1) {
        perror("Could not remove public fifo");
    }

    pthread_exit(NULL);
}
