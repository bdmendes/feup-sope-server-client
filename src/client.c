#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "client/input_validation/input_validation.h"
#include "common/fifo/fifo.h"
#include "common/message/message.h"
#include "common/timer/timer.h"
#include "common/utils/utils.h"

static char public_fifo_name[PATH_MAX];
static int public_fifo_fd = -1;

typedef struct {
    int load;
    int rid;
    struct timeval private_fifo_timeout;
} Request;

void *request_server(void *arg) {
    /* Assemble message to send */
    Message sent_msg;
    Request request = *((Request *)arg);
    assemble_message(&sent_msg, request.rid, request.load, -1);

    /* Make private fifo */
    char private_fifo_name[PATH_MAX];
    get_private_fifo_name(private_fifo_name);
    if (mkfifo(private_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Could not make private fifo");
        return NULL;
    }

    int private_fifo_fd = open(private_fifo_name, O_RDONLY | O_NONBLOCK);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        unlink(private_fifo_name);
        return NULL;
    }

    /* Request via public fifo */
    if (write(public_fifo_fd, &sent_msg, sizeof(sent_msg)) == -1) {
        perror("Could not write to public fifo");
        unlink(private_fifo_name);
        return NULL;
    }

    /* Log request */
    log_operation(IWANT, request.rid, request.load, -1);

    /* Read server response from private fifo */
    Message received_msg;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(private_fifo_fd, &set);
    int s = select(private_fifo_fd + 1, &set, NULL, NULL,
                   &request.private_fifo_timeout);
    if (s == -1) {
        perror("Could not wait for private fifo read");
        return NULL;
    } else if (s == 0) {
        log_operation(GAVUP, request.rid, request.load, -1);
    } else {
        if (read(private_fifo_fd, &received_msg, sizeof(Message)) == -1) {
            perror("Could not read from private fifo");
        } else {
            log_operation(received_msg.tskres == -1 ? CLOSD : GOTRS,
                          request.rid, request.load, received_msg.tskres);
        }
    }

    /* Close and remove private fifo */
    if (close(private_fifo_fd) == -1) {
        perror("Could not close private fifo");
        return NULL;
    }
    if (unlink(private_fifo_name) == -1) {
        perror("Could not remove private fifo");
        return NULL;
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (!valid_client_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> <fifoname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int nsecs = atoi(optarg);
    setup_timer(nsecs);

    /* Open public fifo for writing */
    snprintf(public_fifo_name, PATH_MAX, "%s", argv[optind]);
    while ((public_fifo_fd = open(public_fifo_name, O_WRONLY)) == -1) {
        if (timer_runout()) {
            fprintf(stderr, "Could not open public fifo\n");
            exit(EXIT_FAILURE);
        }
        usleep(BUSY_WAIT_DELAY_MICROS);
    }

    pthread_t id;
    int request_counter = 0;
    unsigned int seed = time(NULL);
    while (!timer_runout()) {
        Request request;
        request.load = 1 + rand_r(&seed) % 9;
        request.rid = request_counter++;
        struct timeval private_fifo_timeout;
        get_timer_remaining_time(&private_fifo_timeout);
        request.private_fifo_timeout = private_fifo_timeout;
        pthread_attr_t tatrr;
        pthread_attr_init(&tatrr);
        pthread_attr_setdetachstate(&tatrr, PTHREAD_CREATE_DETACHED);
        if (pthread_create(&id, &tatrr, request_server, (void *)&request) !=
            0) {
            perror("Could not create thread");
        }
        pthread_attr_destroy(&tatrr);
        int delay = rand_r(&seed);
        usleep(1000 + delay % 50000);
    }

    pthread_exit(NULL);
}