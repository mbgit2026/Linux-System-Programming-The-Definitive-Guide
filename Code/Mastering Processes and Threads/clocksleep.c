#include <stdio.h>
#include <time.h>
#include <errno.h>

int main() {
    struct timespec t;

    // Get current time from CLOCK_MONOTONIC
    clock_gettime(CLOCK_MONOTONIC, &t);

    // Sleep until 5 seconds from now
    t.tv_sec += 5;

    printf("Sleeping until an absolute time (5 seconds from now)...\n");

    int ret = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);

    if (ret == 0) {
        printf("Woke up at the requested absolute time.\n");
    } else {
        printf("clock_nanosleep error: %d\n", ret);
    }

    return 0;
}