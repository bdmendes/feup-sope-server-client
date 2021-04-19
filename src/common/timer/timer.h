#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <stdbool.h>
#include <time.h>

#define BUSY_WAIT_DELAY_MICROS 20000

int setup_timer(unsigned long seconds);

int get_timer_remaining_time(struct timespec *time);

bool time_is_up(const struct timespec *time);

#endif