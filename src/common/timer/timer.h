#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <stdbool.h>
#include <time.h>

#define BUSY_WAIT_DELAY_MICROS 20000

void start_timer(time_t seconds);

time_t get_timer_remaining_seconds();

#endif