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

#define DEFAULT_BUF_SIZE 100

int listener(char fifo_name[]) {
    /* Open public fifo */
    int public_fifo_fd = open(fifo_name, O_RDONLY | O_NONBLOCK);
    if (public_fifo_fd == -1) {
        fprintf(stderr, "Could not open fifo\n");
        return -1;
    }

    /* Initialize detached thread attributes */
    pthread_t id;
    pthread_attr_t tatrr;
    if (pthread_attr_init(&tatrr) != 0) {
        perror("Could not initialize pthread attribute");
        return -1;
    }
    if (pthread_attr_setdetachstate(&tatrr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("Could not set pthread attribute");
        pthread_attr_destroy(&tatrr);
        return -1;
    }

    while (true) {
        /* Assemble remaining time */
        struct timespec remaining_time;
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not set timeout\n");
            continue;
        }
        if (time_is_up(&remaining_time)) {
            pc_signal_server_closed();
            break;
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
        }
        bool public_fifo_timeout = ready_fds == 0;
        if (public_fifo_timeout) {
            printf("hey!!!");
            fflush(stdout);
            break;
        }

        /* Read request and dispatch it */
        Message message;
        if (read(public_fifo_fd, &message, sizeof(Message)) !=
            sizeof(Message)) {
            printf("hey2!!!");
            fflush(stdout);
            break;
        }
        log_operation(RECVD, message.rid, message.tskload, message.tskres);
        push_pending_request(&message);
        if (pthread_create(&id, &tatrr, producer, NULL) != 0) {
            perror("Could not create producer thread");
        }
    }

    /* Destroy thread attributes */
    if (pthread_attr_destroy(&tatrr) == -1) {
        perror("Could not destroy detached producer attributes");
        close(public_fifo_fd);
        return -1;
    }

    /* Close public fifo */
    if (close(public_fifo_fd) == -1) {
        perror("Could not close public fifo");
        return -1;
    }

    return 0;
}

void cleanup() {
    destroy_producer_consumer();
    destroy_timer();
}

int main(int argc, char **argv) {
    if (!valid_server_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> [-l bufsz] <fifoname>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    int nsecs = atoi(argv[2]);
    char *fifo_name = argv[argc - 1];
    int buf_size = argc == 6 ? atoi(argv[4]) : DEFAULT_BUF_SIZE;

    if (mkfifo(fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not create public fifo '%s'\n", fifo_name);
            exit(EXIT_FAILURE);
        }
    }

    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    if (init_producer_consumer(buf_size) != 0) {
        fprintf(stderr, "Could not init producer consumer\n");
        destroy_timer();
        exit(EXIT_FAILURE);
    }

    atexit(cleanup);

    pthread_t consumer_tid;
    if (pthread_create(&consumer_tid, NULL, consumer, NULL) != 0) {
        perror("Could not create consumer thread");
        exit(EXIT_FAILURE);
    }

    if (listener(fifo_name) == -1) {
        fprintf(stderr, "Listener failed\n");
    }

    if (pthread_join(consumer_tid, NULL) == -1) {
        perror("Could not wait for consumer thread to finish");
    }

    fflush(stdout);

    pthread_exit(NULL);
}