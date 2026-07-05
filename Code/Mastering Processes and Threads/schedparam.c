#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <string.h>

int main() {
    struct sched_param param;
    int policy;
    int min, max;

    printf("=== Real-Time Priority Range (SCHED_RR) ===\n");

    min = sched_get_priority_min(SCHED_RR);
    max = sched_get_priority_max(SCHED_RR);

    if (min == -1 || max == -1) {
        perror("sched_get_priority_min/max");
        return 1;
    }

    printf("Min: %d\n", min);
    printf("Max: %d\n\n", max);

    printf("=== Changing Policy to SCHED_RR ===\n");

    param.sched_priority = min;  // required when switching to RT

    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        perror("sched_setscheduler");
        printf("Run as root (sudo).\n");
        return 1;
    }

    printf("Policy changed successfully.\n\n");

    printf("=== Current Policy ===\n");
    policy = sched_getscheduler(0);

    if (policy == -1) {
        perror("sched_getscheduler");
        return 1;
    }

    if (policy == SCHED_RR)
        printf("Current policy: SCHED_RR\n\n");
    else
        printf("Current policy: %d\n\n", policy);

    printf("=== Changing Priority Using sched_setparam ===\n");

    param.sched_priority = max;  // set highest RT priority

    if (sched_setparam(0, &param) == -1) {
        perror("sched_setparam");
        return 1;
    }

    printf("Priority changed successfully.\n\n");

    printf("=== Verifying New Parameters ===\n");

    if (sched_getparam(0, &param) == -1) {
        perror("sched_getparam");
        return 1;
    }

    printf("Current sched_priority: %d\n", param.sched_priority);

    return 0;
}

