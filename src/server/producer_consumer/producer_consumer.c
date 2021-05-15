#include "producer_consumer.h"
#include <pthread.h>
#include <stdio.h>

#include "../lib/lib.h"
#include "../message_queue/message_queue.h"

static MessageQueue *pending;
static MessageQueue *ready;
static unsigned max_pending_size = 0;
static pthread_mutex_t pending_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pending_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

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
                // private fifo info to send
    pthread_mutex_lock(&ready_mutex);
    message_queue_push(ready,
                       &message); // should we also limit the ready queue size?
    pthread_cond_signal(&ready_cond);
    pthread_mutex_unlock(&ready_mutex);
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    pthread_exit(NULL);
}

void destroy_producer_consumer() {
    destroy_message_queue(ready);
    destroy_message_queue(pending);
}