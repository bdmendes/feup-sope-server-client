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

static int public_fifo_fd = -1;

int main(int argc, char **argv) {
    // TO DO: listener function
    // TO DO: make the tests pass (check pc logic)
    if (!valid_server_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> [-l bufsz] <fifoname>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    int nsecs = atoi(argv[2]);
    char *fifo_name = argv[argc - 1];
    int buf_size = 10;
    if (argc == 6)
        buf_size = atoi(argv[4]);

    printf("nsecs = %d, fifoname = %s, bufsz = %d\n", nsecs, fifo_name,
           buf_size);

    struct timespec remaining_time;

    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    if (mkfifo(fifo_name, 0666) != 0) {
        if (errno != EEXIST) {
            fprintf(stderr, "Could not creat the fifo\n");
            exit(EXIT_FAILURE);
        }
    }

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

    if (init_producer_consumer(buf_size) != 0) {
        fprintf(stderr, "Could not init producer consumer\n");
        exit(EXIT_FAILURE);
    }

    public_fifo_fd = open(fifo_name, O_RDONLY);
    if (public_fifo_fd == -1) {
        fprintf(stderr, "Could not open fifo\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&id, &tatrr, consumer, NULL) != 0) {
        perror("Could not create thread");
        exit(EXIT_FAILURE);
    }

    while (true) {
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not set timeout\n");
            continue;
        }
        Message message;
        // open in non block and use select with remaining time + 1 sec for eg!!
        if (read(public_fifo_fd, &message, sizeof(message)) <= 0) {
            if (time_is_up(&remaining_time)) {
                break;
            } else {
                usleep(BUSY_WAIT_DELAY_MICROS);
                continue;
            }
        }
        log_operation(RECVD, message.rid, message.tskload, message.tskres);
        push_pending_request(&message);
        if (pthread_create(&id, &tatrr, producer, NULL) != 0) {
            perror("Could not create producer thread");
        }
    }

    // destroy_timer(); do it only after pc ends...

    if (close(public_fifo_fd) != 0) {
        perror("Could not close the fifo\n");
    }

    atexit(destroy_producer_consumer);
    pthread_exit(NULL);
}