#include "timer.h"

static time_t initial_instant = 0;
static time_t duration = 0;

void start_timer(time_t seconds) {
    initial_instant = time(NULL);
    duration = seconds;
}

time_t get_timer_remaining_seconds() {
    time_t curr_instant = time(NULL);
    return curr_instant - initial_instant > duration
               ? 0
               : duration - (curr_instant - initial_instant);
}