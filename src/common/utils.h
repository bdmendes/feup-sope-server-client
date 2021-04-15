#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

void get_private_fifo_name(char buf[], int pid, int tid);
bool is_all_digits(char *string);

#endif