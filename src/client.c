#include "client/fifo/fifo.h"
#include "common/utils/utils.h"
#include "common/logs/logs.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void *func_1(void *a) {
	//test logs
	OPERATION ot = IWANT;
  	op_reg(4, 5, ot, 6);

    /*
	//teste fifo
	char fifo_name[PATH_MAX];
	char buf[PATH_MAX];
	communication(buf, 4, 7, getpid(), pthread_self(), 4);
	printf("%s", buf);
    get_private_fifo_name(fifo_name, getpid(), pthread_self());
    make_private_fifo(fifo_name);
    delete_private_fifo(fifo_name);
    make_private_fifo(fifo_name);*/
    pthread_exit(NULL);
}

int main() {
    pthread_t id1;
    if (pthread_create(&id1, NULL, func_1, NULL) != 0)
        exit(-1);
    pthread_join(id1, NULL);
    return 0;
}