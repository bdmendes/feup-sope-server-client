#ifndef COMMON_TIMER_H
#define COMMON_TIMER_H

#include <stdbool.h>
#include <time.h>

#define BUSY_WAIT_DELAY_MICROS 20000

/**
 * @brief Create a one shot timer, triggering no signal,
 * and start it with duration equal to seconds.
 * 
 * @param seconds duration of the timer
 * @return 0 if setup successful
 */
int setup_timer(unsigned long seconds);

/**
 * @brief Get the running timer remaining time into
 * struct time. Fails if no timer is running.
 * 
 * @param time struct to put the remaining time on
 * @return 0 if timer is running and struct was set
 */
int get_timer_remaining_time(struct timespec *time);

/**
 * @brief Check if both of the time struct member fields are zero.
 * 
 * @param time to check if zero
 * @return true if both of the time struct member fields are zero.
 */
bool time_is_up(const struct timespec *time);

/**
 * @brief Cleanup routine to delete the previously set up timer
 * 
 */
void destroy_timer();

#endif