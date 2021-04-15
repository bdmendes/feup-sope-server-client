#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
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

char public_fifo_name[PATH_MAX];
pthread_mutex_t public_fifo_mutex;

void *request_server(void *a) {
    /* Assemble message to send */
    Message msg;
    unsigned int seed = 0;
    int load = 1 + rand_r(&seed) % 9;
    int tid = pthread_self();
    int pid = getpid();
    int rid = pid;
    if (assemble_message(&msg, rid, pid, tid, load, -1) != 0) {
        return NULL;
    }

    /* Make private fifo */
    char private_fifo_name[PATH_MAX];
    get_private_fifo_name(private_fifo_name, getpid(), pthread_self());
    if (mkfifo(private_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        perror("Could not make private fifo");
        return NULL;
    }

    /* Enter critical region */
    pthread_mutex_lock(&public_fifo_mutex);

    /* Log request */
    log_operation(IWANT, rid, load, pid, tid, -1);

    /* Request via public fifo */
    int public_fifo_fd = open(public_fifo_name, O_WRONLY);
    if (public_fifo_fd == -1) {
        perror("Could not open public fifo");
        pthread_mutex_unlock(&public_fifo_mutex);
        return NULL;
    }
    if (write(public_fifo_fd, &msg, sizeof(msg)) == -1) {
        perror("Could not write to public fifo");
        pthread_mutex_unlock(&public_fifo_mutex);
        return NULL;
    }
    if (close(public_fifo_fd) == -1) {
        perror("Could not close public fifo");
        pthread_mutex_unlock(&public_fifo_mutex);
        return NULL;
    }

    /* Exit critical region */
    pthread_mutex_unlock(&public_fifo_mutex);

    /* Read server response from private fifo */
    printf("hey!");
    fflush(stdout);
    int private_fifo_fd = open(private_fifo_name, O_RDONLY);
    if (private_fifo_fd == -1) {
        perror("Could not open private fifo");
        return NULL;
    }
    if (read(private_fifo_fd, &msg, sizeof(Message)) == -1) {
        perror("Could not read from private fifo");
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

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (!valid_client_options(argc, argv)) {
        fprintf(stderr, "Usage: %s <-t nsecs> <fifoname>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // int nsecs = atoi(optarg);
    srand(time(NULL));
    snprintf(public_fifo_name, PATH_MAX, "%s", argv[optind]);
    if (mkfifo(public_fifo_name, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
        if (errno != EEXIST) {
            perror("Could not make public fifo");
            exit(EXIT_FAILURE);
        }
    }

    pthread_mutex_init(&public_fifo_mutex, NULL);

    pthread_t id1;
    if (pthread_create(&id1, NULL, request_server, NULL) != 0)
        exit(-1);
    pthread_join(id1, NULL);
    return 0;

    pthread_mutex_destroy(&public_fifo_mutex);
    unlink(public_fifo_name);
}