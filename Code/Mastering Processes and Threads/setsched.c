#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <sys/resource.h>

void print_scheduler(int policy) {
    switch (policy) {
        case SCHED_OTHER:
            printf("Scheduler: SCHED_OTHER (normal)\n");
            break;
        case SCHED_FIFO:
            printf("Scheduler: SCHED_FIFO (real-time FIFO)\n");
            break;
        case SCHED_RR:
            printf("Scheduler: SCHED_RR (real-time round robin)\n");
            break;
        default:
            printf("Scheduler: Unknown (%d)\n", policy);
    }
}

int main() {
    int policy;
    struct sched_param param;

    printf("=== Initial Scheduler ===\n");
    policy = sched_getscheduler(0);
    if (policy == -1) {
        perror("sched_getscheduler");
        return 1;
    }
    print_scheduler(policy);

    printf("\n=== Current Nice Value ===\n");
    int nice_val = getpriority(PRIO_PROCESS, 0);
    printf("Nice value: %d\n", nice_val);

    printf("\n=== Updating Nice ===\n");
    if (nice(5) == -1 && errno != 0) 
        perror("nice");
    

    nice_val = getpriority(PRIO_PROCESS, 0);
    printf("New nice value: %d\n", nice_val);

    printf("\n=== Attempting to set SCHED_RR ===\n");
    param.sched_priority = 10; // real-time priority (1-99 typically)

    if (sched_setscheduler(0, SCHED_RR, &param) == -1) {
        printf("sched_setscheduler failed: %s\n", strerror(errno));
        printf("(You probably need root privileges)\n");
    } else {
        printf("Scheduler changed successfully\n");
    }

    printf("\n=== Final Scheduler ===\n");
    policy = sched_getscheduler(0);
    print_scheduler(policy);

    return 0;
}
