#include "producer_consumer.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../common/fifo/fifo.h"
#include "../../common/log/log.h"
#include "../../common/timer/timer.h"
#include "../lib/lib.h"
#include "../message_queue/message_queue.h"

static MessageQueue *pending;
static MessageQueue *ready;
static unsigned max_buffer_size = 0;
static pthread_mutex_t pending_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pending_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;

static bool server_closed = false;

int init_producer_consumer(unsigned buffer_size) {
    pending = init_message_queue();
    ready = init_message_queue();
    max_buffer_size = buffer_size;
    if (pending == NULL || ready == NULL) {
        fprintf(stderr, "Could not initialize producer consumer queues");
        return -1;
    }
    return 0;
}

int push_pending_request(const Message *message) {
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_size(pending) >= max_buffer_size) {
        pthread_cond_wait(&pending_cond,
                          &pending_mutex); // TO DO: check for timeout
    }
    message_queue_push(pending, message);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);
    return 0;
}

void *producer(void *arg) {
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_empty(pending)) {
        pthread_cond_wait(&pending_cond, &pending_mutex);
    }
    Message message = message_queue_front(pending);
    message_queue_pop(pending);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);

    if (!server_closed) {
        struct timespec t;
        get_timer_remaining_time(&t);
        if (time_is_up(&t))
            server_closed = true;
    }

    if (!server_closed) {
        message.tskres = task(message.tskload);
        log_operation(TSKEX, message.rid, message.tskload, message.tskres);
    }

    pthread_mutex_lock(&ready_mutex);
    while (message_queue_size(ready) >= max_buffer_size) {
        pthread_cond_wait(&ready_cond, &ready_mutex);
    }
    message_queue_push(ready, &message);
    pthread_cond_signal(&ready_cond);
    pthread_mutex_unlock(&ready_mutex);
    pthread_exit(NULL);
}

void *consumer(void *arg) {

    while (true) {
        pthread_mutex_lock(&ready_mutex);
        while (message_queue_empty(ready)) {
            if (server_closed) {
                pthread_mutex_unlock(&ready_mutex);
                pthread_exit(NULL);
            }
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
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
            usleep(BUSY_WAIT_DELAY_MICROS);
        } else {
            message.pid = getpid();
            message.tid = pthread_self();

            if (write(private_fifo_fd, &message, sizeof(message)) == -1) {
                log_operation(FAILD, message.rid, message.tskload,
                              message.tskres);
            } else {
                log_operation(message.tskres == -1 ? TOOLATE : TSKDN,
                              message.rid, message.tskload, message.tskres);
            }
            close(private_fifo_fd);
        }
    }
    pthread_exit(NULL);
}

void destroy_producer_consumer() {
    destroy_message_queue(ready);
    destroy_message_queue(pending);
}
