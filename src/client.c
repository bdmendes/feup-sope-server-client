/*#include <stdio.h>
#include "op_register.h"

int main() {
  OPERATION ot = IWANT;
  op_reg(4, 5, ot, 6);
  return 0;
}
*/
#include <stdio.h>

#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "op_register.h"

void *func_1(void *a) {
	OPERATION ot = IWANT;
  	op_reg(4, 5, ot, 6);
	pthread_exit(NULL);	// no termination code
}

int main() {
	pthread_t id1;
	if (pthread_create(&id1, NULL, func_1, NULL) != 0)
		exit(-1);
	pthread_join(id1, NULL);
	return 0;
  
}