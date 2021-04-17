#include "timer.h"
#include <signal.h>
#include <string.h>
#include <unistd.h>

static struct timeval initial_instant;
static struct timeval curr_instant;
static time_t duration_seconds = 0;
static bool runout = false;

void alarm_handler() {
    runout = true;
}

void start_timer(unsigned long seconds) {
    gettimeofday(&initial_instant, NULL);
    runout = false;

    struct sigaction sig;
    memset(&sig, 0, sizeof(struct sigaction));
    sig.sa_flags = SA_RESTART; // don't make read/open calls fail
    sig.sa_handler = alarm_handler;
    sigemptyset(&sig.sa_mask);
    sigaction(SIGALRM, &sig, NULL);

    alarm(seconds);
}

void get_timer_remaining_time(struct timeval *timeval) {
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
    return runout;
}