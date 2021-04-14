#include "client/fifo/fifo.h"
#include "common/utils.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "op_register.h"
#include <dirent.h>

/*void *func_1(void *a) {
	OPERATION ot = IWANT;
  	op_reg(4, 5, ot, 6);
	pthread_exit(NULL);	// no termination code
}*/


void *func_1(void *a) {
    char fifo_name[PATH_MAX];
    get_private_fifo_name(fifo_name, getpid(), pthread_self());
    make_private_fifo(fifo_name);
    delete_private_fifo(fifo_name);
    make_private_fifo(fifo_name);
    pthread_exit(NULL);
}

int main() {
    pthread_t id1;
    if (pthread_create(&id1, NULL, func_1, NULL) != 0)
        exit(-1);
    pthread_join(id1, NULL);
    return 0;
}