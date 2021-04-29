#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client/parser/parser.h"
#include "client/request/request.h"
#include "common/timer/timer.h"

static int public_fifo_fd = -1;

/**
 * @brief Try to open the public fifo within the time given to execute
 * @param public_fifo_name Name of the public fifo
 * @return 0 upon success, -1 otherwise
 */

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

/**
 * @brief Function to close the public fifo
 */
void close_public_fifo() {
    if (close(public_fifo_fd) == -1) {
        perror("Could not close public fifo");
    }
}

/**
 * @brief Sleep some random miliseconds.
 *
 * @param seed to pass to rand_r
 */
void sleep_couple_ms(unsigned int *seed) {
    int delay = rand_r(seed);
    usleep(10000 + delay % 40000);
}

/**
 * @brief Function to creat the multiple threads
 */
int spawn_request_threads() {
    struct timespec remaining_time;
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
    int request_counter = 0;
    unsigned int seed = time(NULL);
    while (!is_server_closed()) {
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not set private fifo read timeout\n");
            sleep_couple_ms(&seed);
            continue;
        }
        if (time_is_up(&remaining_time)) {
            break;
        }
        Request *request = (Request *)malloc(sizeof(Request));
        if (request == NULL) {
            fprintf(stderr, "Could not allocate request struct\n");
            sleep_couple_ms(&seed);
            continue;
        }
        request->load = 1 + rand_r(&seed) % 9;
        request->rid = request_counter++;
        request->public_fifo_fd = public_fifo_fd;
        request->private_fifo_timeout = remaining_time;
        if (pthread_create(&id, &tatrr, request_server, request) != 0) {
            perror("Could not create thread");
            free(request);
        }
        sleep_couple_ms(&seed);
    }
    if (pthread_attr_destroy(&tatrr) != 0) {
        perror("Could not destroy pthread attribute");
        return -1;
    }
    return 0;
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

    if (spawn_request_threads() == -1) {
        fprintf(stderr, "Could not spawn all worker threads\n");
    }

    destroy_timer();
    atexit(close_public_fifo);
    pthread_exit(NULL);
}
