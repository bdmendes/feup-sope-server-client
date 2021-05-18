#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "server/message_queue/message_queue.h"
#include "common/log/log.h"
#include "common/timer/timer.h"
#include "server/producer_consumer/producer_consumer.h"

static int public_fifo_fd = -1;

int main(){
    /*Testing values*/
    int nsecs = 5;
    char name[20] = "/tmp/a_fifo";
    unsigned buf_size = 10;

    struct timespec remaining_time;

    if (setup_timer(nsecs) == -1) {
        exit(EXIT_FAILURE);
    }

    if(mkfifo(name, 0666) != 0){
        if(errno != EEXIST){
            fprintf(stderr, "Could not creat the fifo\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_t id;
    pthread_attr_t tatrr;
    if (pthread_attr_init(&tatrr) != 0) {
        perror("Could not initialize pthread attribute");
        return -1;
    }
    if (pthread_attr_setdetachstate(&tatrr, PTHREAD_CREATE_DETACHED) != 0) {
        perror("Could not set pthread attribute");
        pthread_attr_destroy(&tatrr);
        return -1;
    }

    if(init_producer_consumer(buf_size) != 0){
        fprintf(stderr, "Could not init producer consumer\n");
        exit(EXIT_FAILURE);
    }

    public_fifo_fd = open(name, O_RDONLY);
    if(public_fifo_fd == -1){
        fprintf(stderr, "Could not open fifo\n");
        exit(EXIT_FAILURE);
    } 

    if (pthread_create(&id, &tatrr, consumer, NULL) != 0) {
        perror("Could not create thread");
        exit(EXIT_FAILURE);
    }
    

    while(true){
        if (get_timer_remaining_time(&remaining_time) == -1) {
            fprintf(stderr, "Could not set timeout\n");
            continue;
        }
        if (time_is_up(&remaining_time)) {
            if(close(public_fifo_fd) != 0){
                perror("Could not close the fifo\n");
            }
            break;
        }
        Message* message = NULL;
        if(read(public_fifo_fd, message, sizeof(message)) == -1){
            fprintf(stderr, "Could not read message\n");
            continue;
        }
        if(message != NULL){
            log_operation(TSKEX, message->rid, message->tskload, message->tskres); //maybe this is not the the answer but that will do for test
            push_pending_request(message);
        }
        if (pthread_create(&id, &tatrr, producer, NULL) != 0) {
            perror("Could not create producer thread");
        }
    }

    destroy_timer();
    atexit(destroy_producer_consumer);
    pthread_exit(NULL);

    /*MessageQueue* queue = init_message_queue();
    Message msg1, msg2;
    assemble_message(&msg1, 1, 1, 4);
    assemble_message(&msg2, 2, 5, 7);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    printf("queue size: %u\n", message_queue_size(queue));
    message_queue_push(queue, &msg1);
    message_queue_push(queue, &msg2);
    printf("queue size: %u\n", message_queue_size(queue));
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    Message msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    msg1.rid = 30; // must not affect the queue
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    message_queue_pop(queue);
    msg_front = message_queue_front(queue);
    log_operation(IWANT, msg_front.rid, msg_front.tskload, msg_front.tskres);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    printf("queue size: %u\n", message_queue_size(queue));
    message_queue_pop(queue);
    printf("is empty: %s\n", message_queue_empty(queue) ? "yes" : "no");
    destroy_message_queue(queue);*/
}