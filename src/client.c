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
#include "common/logs/logs.h"
#include "common/message/message.h"
#include "common/utils/utils.h"

static char public_fifo_name[PATH_MAX];
static int public_fifo_fd = -1;
static pthread_mutex_t public_fifo_mutex;

void *request_server(void *a) {
    /* Assemble message to send */
    Message msg;
    unsigned int seed = time(NULL);
    int load = 1 + rand_r(&seed) % 9;
    pthread_t tid = pthread_self();
    pid_t pid = getpid();
    int rid = tid % pid;
    if (assemble_message(&msg, rid, load, pid, tid, -1) != 0) {
        return NULL;
    }

    /* Make private fifo */
    char private_fifo_name[PATH_MAX];
    get_private_fifo_name(private_fifo_name, getpid(), pthread_self());
    if (mkfifo(private_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Could not make private fifo");
        return NULL;
    }

    /* Request via public fifo */
    pthread_mutex_lock(&public_fifo_mutex);
    if (write(public_fifo_fd, &msg, sizeof(msg)) == -1) {
        perror("Could not write to public fifo");
        unlink(private_fifo_name);
        pthread_mutex_unlock(&public_fifo_mutex);
        return NULL;
    }
    pthread_mutex_unlock(&public_fifo_mutex);

    /* Log request */
    log_operation(IWANT, rid, load, pid, tid, -1);

    /* Read server response from private fifo */
    int private_fifo_fd = open(private_fifo_name, O_RDONLY);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        unlink(private_fifo_name);
        return NULL;
    }
    if (read(private_fifo_fd, &msg, sizeof(Message)) == -1) {
        perror("Could not read from private fifo");
        close(private_fifo_fd);
        unlink(private_fifo_name);
        return NULL;
    }

    /* Log server response */
    log_operation(msg.tskres == -1 ? CLOSD : GOTRS, rid, load, pid, tid,
                  msg.tskres);

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
    if (mkfifo(public_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        if (errno != EEXIST) {
            perror("Could not make public fifo");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_init(&public_fifo_mutex, NULL);

    /* Open public fifo for writing */
    public_fifo_fd = open(public_fifo_name, O_WRONLY);
    if (public_fifo_fd == -1) {
        perror("Could not open public fifo");
        pthread_mutex_unlock(&public_fifo_mutex);
        exit(EXIT_FAILURE);
    }

    // NOT HANDLING TIMEOUT YET
    pthread_t id[1000];
    unsigned int seed = time(NULL);
    for (int i = 0; i < 1000; i++) {
        if (pthread_create(&id[i], NULL, request_server, NULL) != 0) {
            perror("Could not create thread");
        }
        int delay = rand_r(&seed);
        usleep(100 + delay % 9000); // TRY WITH SMALLER DELAYS TO CHECK READ:
                                    // TOO MANY FILES BLOCK
    }
    for (int i = 0; i < 1000; i++) {
        if (pthread_join(id[i], NULL) != 0) {
            perror("Could not join thread");
        }
    }
    return 0;

    /* Close public fifo */
    if (close(public_fifo_fd) == -1) {
        perror("Could not close private fifo");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&public_fifo_mutex);
}