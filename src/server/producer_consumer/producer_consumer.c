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

/* Data structures */
static MessageQueue *pending;
static MessageQueue *ready;

/* Flow control */
static int producer_count = 0;
static volatile bool server_closed = false;
static unsigned max_buffer_size = 0;

/* Auxiliary buffers */
static pthread_t consumer_tid;
static pthread_attr_t producer_tatrr;

/* Sync primitives */
static pthread_mutex_t pending_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t pending_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t ready_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t pcount_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Function to get an absolute instant a bit after the timeout ends
 * @param t Time structure to return
 */
static void get_relaxed_future_runout(struct timespec *t) {
    struct timespec curr_time, remaining_time;
    get_timer_remaining_time(&remaining_time);
    clock_gettime(CLOCK_REALTIME, &curr_time);
    curr_time.tv_sec += 1 + remaining_time.tv_sec;
    *t = curr_time;
}

/**
 * @brief Producer thread boddy
 * @param arg Arguments
 */
static void *producer(void *arg) {
    /* Increment producer count */
    pthread_mutex_lock(&pcount_mutex);
    producer_count += 1;
    pthread_mutex_unlock(&pcount_mutex);

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
        pthread_cond_wait(&ready_cond, &ready_mutex);
    }
    message_queue_push(ready, &message);
    pthread_cond_signal(&ready_cond);
    pthread_mutex_unlock(&ready_mutex);

    /* Decrement producer count */
    pthread_mutex_lock(&pcount_mutex);
    producer_count -= 1;
    pthread_mutex_unlock(&pcount_mutex);

    pthread_exit(NULL);
}

/**
 * @brief Consumer thread boddy
 * @param arg Arguments
 */
static void *consumer(void *arg) {
    while (true) {
        /* Wait for a ready answer with relaxed timeout */
        pthread_mutex_lock(&ready_mutex);
        while (message_queue_empty(ready)) {
            struct timespec future;
            get_relaxed_future_runout(&future);
            if (pthread_cond_timedwait(&ready_cond, &ready_mutex, &future) ==
                ETIMEDOUT) {
                if (!message_queue_empty(ready)) {
                    break;
                }
                pthread_mutex_lock(&pcount_mutex);
                if (producer_count == 0) {
                    pthread_mutex_unlock(&pcount_mutex);
                    pthread_mutex_unlock(&ready_mutex);
                    pthread_exit(NULL);
                } else {
                    pthread_mutex_unlock(&pcount_mutex);
                    continue;
                }
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
        if ((private_fifo_fd =
                 open(private_fifo_name, O_WRONLY | O_NONBLOCK)) == -1) {
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
            usleep(BUSY_WAIT_DELAY_MICROS);
            continue;
        }

        /* Set thread identification */
        message.pid = getpid();
        message.tid = pthread_self();

        /* Send answer via private fifo */
        struct timespec remaining_time;
        get_timer_remaining_time(&remaining_time);
        remaining_time.tv_sec += 1;
        fd_set set;
        FD_ZERO(&set);
        FD_SET(private_fifo_fd, &set);
        int ready_fds = pselect(private_fifo_fd + 1, NULL, &set, NULL,
                                &remaining_time, NULL);
        if (ready_fds <= 0) {
            log_operation(FAILD, message.rid, message.tskload, message.tskres);
            usleep(BUSY_WAIT_DELAY_MICROS);
        } else {
            if (write(private_fifo_fd, &message, sizeof(message)) <
                sizeof(Message)) {
                log_operation(FAILD, message.rid, message.tskload,
                              message.tskres);
            } else {
                log_operation(message.tskres == -1 ? TOOLATE : TSKDN,
                              message.rid, message.tskload, message.tskres);
            }
        }

        /* Close private fifo */
        if (close(private_fifo_fd) == -1) {
            fprintf(stderr, "Could not close private fifo '%s'\n",
                    private_fifo_name);
        }
    }

    pthread_exit(NULL);
}

/**
 * @brief Function to creat producer thread
 * @return 0 uppon sucess, -1 otherwise
 */
static int spawn_producer() {
    pthread_t id;
    if (pthread_create(&id, &producer_tatrr, producer, NULL) != 0) {
        perror("Could not create producer thread");
        return -1;
    }
    return 0;
}

int pc_init(unsigned buffer_size) {
    /* Init data structures */
    pending = init_message_queue();
    ready = init_message_queue();
    max_buffer_size = buffer_size > 0 ? buffer_size : DEFAULT_BUF_SIZE;
    if (pending == NULL || ready == NULL) {
        fprintf(stderr, "Could not initialize producer consumer queues");
        return -1;
    }

    /* Start consumer thread */
    if (pthread_create(&consumer_tid, NULL, consumer, NULL) != 0) {
        perror("Could not create consumer thread");
        return -1;
    }

    /* Prepare producer thread attributes */
    if (pthread_attr_init(&producer_tatrr) != 0) {
        perror("Could not initialize pthread attribute");
        return -1;
    }
    if (pthread_attr_setdetachstate(&producer_tatrr, PTHREAD_CREATE_DETACHED) !=
        0) {
        perror("Could not set pthread attribute");
        pthread_attr_destroy(&producer_tatrr);
        return -1;
    }

    return 0;
}

void pc_push_pending_request(const Message *message) {
    pthread_mutex_lock(&pending_mutex);
    while (message_queue_size(pending) >= max_buffer_size) {
        pthread_cond_wait(&pending_cond, &pending_mutex);
    }
    message_queue_push(pending, message);
    pthread_cond_broadcast(&pending_cond);
    pthread_mutex_unlock(&pending_mutex);
    spawn_producer();
}

void pc_signal_server_closed() {
    server_closed = true;
}

void pc_destroy() {
    if (pthread_join(consumer_tid, NULL) == -1) {
        perror("Could not wait for consumer thread to finish");
    }
    destroy_message_queue(ready);
    destroy_message_queue(pending);
    if (pthread_attr_destroy(&producer_tatrr) == -1) {
        perror("Could not destroy producer thread attributes");
    }
}
