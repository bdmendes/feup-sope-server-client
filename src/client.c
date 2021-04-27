#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client/parser/parser.h"
#include "client/request/request.h"
#include "common/timer/timer.h"

static int public_fifo_fd = -1;

int open_public_fifo(char public_fifo_name[]) {
    struct timespec remaining_time;
    while ((public_fifo_fd = open(public_fifo_name, O_WRONLY | O_CLOEXEC)) ==
           -1) {
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not wait for public fifo opening\n");
            return -1;
        }
        if (time_is_up(&remaining_time)) {
            fprintf(stderr, "Could not open public fifo: Timeout\n");
            return -1;
        }
        usleep(BUSY_WAIT_DELAY_MICROS);
    }
    return 0;
}

void close_public_fifo() {
    if (close(public_fifo_fd) == -1) {
        perror("Could not close public fifo");
    }
}

void spawn_request_threads() {
    struct timespec remaining_time;
    pthread_t id;
    pthread_attr_t tatrr;
    pthread_attr_init(&tatrr);
    pthread_attr_setdetachstate(&tatrr, PTHREAD_CREATE_DETACHED);
    int request_counter = 0;
    unsigned int seed = time(NULL);
    while (!is_server_closed()) {
        Request *request = (Request *)malloc(sizeof(Request));
        request->load = 1 + rand_r(&seed) % 9;
        request->rid = request_counter++;
        request->public_fifo_fd = public_fifo_fd;
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not set private fifo read timeout\n");
        } else {
            if (time_is_up(&remaining_time)) {
                break;
            }
            request->private_fifo_timeout = remaining_time;
            if (pthread_create(&id, &tatrr, request_server, (void *)request) !=
                0) {
                perror("Could not create thread");
            }
        }
        int delay = rand_r(&seed);
        usleep(10000 + delay % 40000);
    }
    pthread_attr_destroy(&tatrr);
}

int main(int argc, char *argv[]) {
    if (!valid_client_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> <fifoname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nsecs = atoi(argv[2]);
    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    if (open_public_fifo(argv[3]) == -1) {
        exit(EXIT_FAILURE);
    }

    spawn_request_threads();

    atexit(close_public_fifo);
    pthread_exit(NULL);
}
