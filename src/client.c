#include <dirent.h>
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

#include "client/parser/parser.h"
#include "common/fifo/fifo.h"
#include "common/log/log.h"
#include "common/message/message.h"
#include "common/timer/timer.h"
#include "common/utils/utils.h"

static char public_fifo_name[PATH_MAX];
static int public_fifo_fd = -1;
static volatile bool server_closed = false;

typedef struct {
    int load;
    int rid;
    struct timespec private_fifo_timeout;
} Request;

void thread_free_request(void *request) {
    free(request);
}

void thread_unlink_fifo(void *fifo_name) {
    char *fifo_name_ptr = (char *)fifo_name;
    if (unlink(fifo_name_ptr) == -1) {
        perror("thread unlink");
    }
}

void thread_close_fifo(void *fd) {
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
    pthread_cleanup_push(thread_free_request, (void *)request);

    /* Make private fifo */
    char private_fifo_name[PATH_MAX];
    get_private_fifo_name(private_fifo_name, getpid(), pthread_self());
    if (mkfifo(private_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Could not make private fifo");
        pthread_exit(NULL);
    }
    pthread_cleanup_push(thread_unlink_fifo, (void *)private_fifo_name);

    /* Open private fifo for reading */
    int private_fifo_fd = open(private_fifo_name, O_RDONLY | O_NONBLOCK);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        pthread_exit(NULL);
    }
    pthread_cleanup_push(thread_close_fifo, (void *)&private_fifo_fd);

    /* Write request to public fifo */
    if (write(public_fifo_fd, &sent_msg, sizeof(sent_msg)) == -1) {
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

int open_public_fifo() {
    struct timespec remaining_time;
    while ((public_fifo_fd = open(public_fifo_name, O_WRONLY)) == -1) {
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
    while (!server_closed) {
        Request *request = (Request *)malloc(sizeof(Request));
        request->load = 1 + rand_r(&seed) % 9;
        request->rid = request_counter++;
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

    int nsecs = atoi(optarg);
    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    snprintf(public_fifo_name, PATH_MAX, "%s", argv[3]);
    if (open_public_fifo() == -1) {
        exit(EXIT_FAILURE);
    }

    spawn_request_threads();

    atexit(close_public_fifo);
    pthread_exit(NULL);
}
