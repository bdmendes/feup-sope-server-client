#include "timer.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static struct timeval initial_instant;
static struct timeval curr_instant;
static time_t duration_seconds = 0;
static bool runout = false;
static bool timer_set = false;

static void alarm_handler() {
    runout = true;
}

void setup_timer(unsigned long seconds) {
    gettimeofday(&initial_instant, NULL);
    timer_set = true;
    runout = false;

    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_flags = SA_RESTART;
    sig.sa_handler = alarm_handler;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGALRM, &sig, NULL);

    alarm(seconds);
}

void get_timer_remaining_time(struct timeval *timeval) {
    if (!timer_set) {
        fprintf(stderr, "Timer is unset");
    }
    if (runout) {
        timeval->tv_sec = 0;
        timeval->tv_usec = 0;
    } else {
        gettimeofday(&curr_instant, NULL);
        timeval->tv_sec =
            duration_seconds - (curr_instant.tv_sec - initial_instant.tv_sec);
        timeval->tv_usec = 1000000000 - curr_instant.tv_usec;
    }
}

bool timer_runout() {
    return timer_set && runout;
}