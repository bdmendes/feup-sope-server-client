#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "client/input_validation/input_validation.h"
#include "common/fifo/fifo.h"
#include "common/message/message.h"
#include "common/utils/utils.h"

static char public_fifo_name[PATH_MAX];
static int public_fifo_fd = -1;

typedef struct {
    int load;
    int rid;
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

    /* Request via public fifo */
    if (write(public_fifo_fd, &sent_msg, sizeof(sent_msg)) == -1) {
        perror("Could not write to public fifo");
        unlink(private_fifo_name);
        return NULL;
    }

    int private_fifo_fd = open(private_fifo_name, O_RDONLY);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        unlink(private_fifo_name);
        return NULL;
    }

    /* Log request */
    log_operation(IWANT, request.rid, request.load, -1);

    /* Read server response from private fifo */
    Message received_msg;
    if (read(private_fifo_fd, &received_msg, sizeof(Message)) == -1) {
        perror("Could not read from private fifo");
        close(private_fifo_fd);
        unlink(private_fifo_name);
        return NULL;
    }

    /* Log server response */
    log_operation(received_msg.tskres == -1 ? CLOSD : GOTRS, request.rid,
                  request.load, received_msg.tskres);

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

    // int nsecs = atoi(optarg);
    snprintf(public_fifo_name, PATH_MAX, "%s", argv[optind]);

    /* Open public fifo for writing */
    public_fifo_fd = open(public_fifo_name, O_WRONLY);
    if (public_fifo_fd == -1) {
        perror("Could not open public fifo");
        exit(EXIT_FAILURE);
    }

    // NOT HANDLING TIMEOUT YET
    pthread_t id[10];
    unsigned int seed = time(NULL);
    for (int i = 0; i < 10; i++) {
        Request request;
        request.load = 1 + rand_r(&seed) % 9;
        request.rid = i;
        if (pthread_create(&id[i], NULL, request_server, (void *)&request) !=
            0) {
            perror("Could not create thread");
        }
        int delay = rand_r(&seed);
        usleep(1000 + delay % 9000);
    }
    for (int i = 0; i < 10; i++) {
        if (pthread_join(id[i], NULL) != 0) {
            perror("Could not join thread");
        }
    }

    /* Close public fifo */
    if (close(public_fifo_fd) == -1) {
        perror("Could not close private fifo");
        exit(EXIT_FAILURE);
    }
}