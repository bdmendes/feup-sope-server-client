#include <stdio.h>
#include "common/utils.h"

#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *func_1(void *a) {
	make_fifo();
  delete_fifo();
  make_fifo();
	pthread_exit(NULL);	// no termination code
}

int main() {
	pthread_t id1;
	if (pthread_create(&id1, NULL, func_1, NULL) != 0)
		exit(-1);
	pthread_join(id1, NULL);
	return 0;
  
}