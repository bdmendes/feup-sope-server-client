#include "producer_consumer.h"
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../common/fifo/fifo.h"
#include "../../common/log/log.h"
#include "../lib/lib.h"
#include "../message_queue/message_queue.h"

static MessageQueue *pending;
static MessageQueue *ready;
static unsigned max_pending_size = 0;
static pthread_mutex_t pending_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pending_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

extern volatile bool server_closed;

int init_producer_consumer(unsigned buffer_size) {
    pending = init_message_queue();
    ready = init_message_queue();
    max_pending_size = buffer_size;
    if (pending == NULL || ready == NULL) {
        fprintf(stderr, "Could not initialize producer consumer queues");
        return -1;
    }
    return 0;
}

int push_pending_request(const Message *message) {
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_size(pending) >= max_pending_size) {
        pthread_cond_wait(&pending_cond,
                          &pending_mutex); // TO DO: check for timeout
    }
    message_queue_push(pending, message);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);
    return 0;
}

void *producer(void *arg) {
    // TO DO: check for return values and cleanup routines
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_empty(pending)) {
        pthread_cond_wait(&pending_cond,
                          &pending_mutex); // TO DO: check for timeout
    }
    Message message = message_queue_front(pending);
    message_queue_pop(pending);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);
    int answer = task(message.tskload);
    message.tskres =
        answer; // it is up to the consumer to change the thread and process ids
                // after popping from the ready queue, after gathering the
                // prsys/stativate fifo info to send

    log_operation(TSKEX, message.rid, message.tskload,
                  message.tskres); // maybe this is not the the answer
                                   // but that will do for test

    pthread_mutex_lock(&ready_mutex);
    message_queue_push(ready,
                       &message); // should we also limit the ready queue size?
    pthread_cond_signal(&ready_cond);
    pthread_mutex_unlock(&ready_mutex);
    pthread_exit(NULL);
}

void *consumer(void *arg) {

    while (true) {
        pthread_mutex_lock(&ready_mutex);
        while (message_queue_empty(ready)) {
            pthread_cond_wait(&ready_cond, &ready_mutex);
        }
        Message message = message_queue_front(ready);
        message_queue_pop(ready);
        pthread_cond_broadcast(&ready_cond);
        pthread_mutex_unlock(&ready_mutex);

        char private_fifo_name[PATH_MAX];
        get_private_fifo_name(private_fifo_name, message.pid, message.tid);
        int private_fifo_fd = open(private_fifo_name, O_WRONLY);
        if (private_fifo_fd == -1) {
            perror("Could not open private fifo");
            pthread_exit(NULL);
        }

        message.pid = getpid();
        message.tid = pthread_self();

        if (server_closed) {
            message.tskres = -1;
        }

        if (write(private_fifo_fd, &message, sizeof(message)) == -1) {
            perror("Could not write to private fifo");
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
            pthread_exit(NULL);
        }

        OPERATION operation = server_closed ? TOOLATE : TSKDN;
        log_operation(operation, message.rid, message.tskload, message.tskres);
    }
    pthread_exit(NULL);
}

void destroy_producer_consumer() {
    destroy_message_queue(ready);
    destroy_message_queue(pending);
}