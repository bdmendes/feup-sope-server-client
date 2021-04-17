#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#define BUSY_WAIT_DELAY_MICROS 20000

void setup_timer(unsigned long seconds);

void get_timer_remaining_time(struct timeval *timeval);

bool timer_runout();

#endif