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
static volatile bool server_closed = false;

static void get_future_time_after_timeout(struct timespec *buf) {
    struct timespec curr_time, remaining_time;
    clock_gettime(CLOCK_MONOTONIC, &curr_time);
    get_timer_remaining_time(&remaining_time);
    curr_time.tv_sec += remaining_time.tv_sec + 1;
    *buf = curr_time;
}

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
        pthread_cond_wait(&pending_cond, &pending_mutex);
    }
    message_queue_push(pending, message);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);
    return 0;
}

void *producer(void *arg) {
    /* Pop pending request queue */
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_empty(pending)) {
        pthread_cond_wait(&pending_cond, &pending_mutex);
    }
    Message message = message_queue_front(pending);
    message_queue_pop(pending);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);

    /* Process request if server is open */
    if (!server_closed) {
        message.tskres = task(message.tskload);
        log_operation(TSKEX, message.rid, message.tskload, message.tskres);
    }

    /* Send request to consumer queue */
    pthread_mutex_lock(&ready_mutex);
    while (message_queue_size(ready) >= max_buffer_size) {
        struct timespec future;
        get_future_time_after_timeout(&future);
        if (pthread_cond_timedwait(&ready_cond, &ready_mutex, &future) ==
            ETIMEDOUT) {
            perror("Consumer took too long to free space on the buffer");
            break;
        }
    }
    message_queue_push(ready, &message);
    pthread_cond_signal(&ready_cond);
    pthread_mutex_unlock(&ready_mutex);

    pthread_exit(NULL);
}

void *consumer(void *arg) {
    while (true) {
        /* Wait for a ready answer with relaxed timeout */
        pthread_mutex_lock(&ready_mutex);
        while (message_queue_empty(ready)) {
            struct timespec future;
            get_future_time_after_timeout(&future);
            if (pthread_cond_timedwait(&ready_cond, &ready_mutex, &future) ==
                ETIMEDOUT) {
                if (!server_closed)
                    continue;
                pthread_mutex_unlock(&ready_mutex);
                pthread_exit(NULL);
            }
        }
        Message message = message_queue_front(ready);
        message_queue_pop(ready);
        pthread_cond_broadcast(&ready_cond);
        pthread_mutex_unlock(&ready_mutex);

        /* Open private fifo */
        char private_fifo_name[PATH_MAX];
        get_private_fifo_name(private_fifo_name, message.pid, message.tid);
        int private_fifo_fd;
        if ((private_fifo_fd = open(private_fifo_name, O_WRONLY)) == -1) {
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
            usleep(BUSY_WAIT_DELAY_MICROS);
            continue;
        }

        /* Send answer via private fifo */
        message.pid = getpid();
        message.tid = pthread_self();
        if (write(private_fifo_fd, &message, sizeof(message)) <
            sizeof(Message)) {
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
        } else {
            log_operation(message.tskres == -1 ? TOOLATE : TSKDN, message.rid,
                          message.tskload, message.tskres);
        }

        /* Close private fifo */
        if (close(private_fifo_fd) == -1) {
            fprintf(stderr, "Could not close private fifo '%s'\n",
                    private_fifo_name);
        }
    }

    pthread_exit(NULL);
}

void pc_signal_server_closed() {
    server_closed = true;
}

void destroy_producer_consumer() {
    destroy_message_queue(ready);
    destroy_message_queue(pending);
}
