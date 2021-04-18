#include "timer.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool timer_set = false;
static timer_t timerid;

int setup_timer(unsigned long seconds) {
    struct sigevent sev;
    struct itimerspec its;

    /* Create the timer */
    sev.sigev_notify = SIGEV_NONE;
    sev.sigev_value.sival_ptr = &timerid;
    if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1) {
        perror("Could not create timer");
        return -1;
    }

    /* Set expiration time and one shot behavior */
    its.it_value.tv_sec = seconds;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        perror("Could not set timer time");
        return -1;
    }

    timer_set = true;
    return 0;
}

int get_timer_remaining_time(struct timespec *time) {
    if (!timer_set) {
        fprintf(stderr, "Timer is unset");
        return -1;
    }
    struct itimerspec its;
    if (timer_gettime(timerid, &its) == -1) {
        perror("Could not get timer remaining time");
        return -1;
    }
    time->tv_sec = its.it_value.tv_sec;
    time->tv_nsec = its.it_value.tv_nsec;
    return 0;
}

bool time_is_up(const struct timespec *time) {
    return time->tv_sec == 0 && time->tv_nsec == 0;
}