#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sched.h>
#include <linux/sched.h>
#include <errno.h>

void print_policy(int policy) {
    switch (policy) {
        case SCHED_OTHER: printf("SCHED_OTHER\n"); break;
        case SCHED_FIFO:  printf("SCHED_FIFO\n");  break;
        case SCHED_RR:    printf("SCHED_RR\n");    break;
        case SCHED_BATCH: printf("SCHED_BATCH\n"); break;
        case SCHED_IDLE:  printf("SCHED_IDLE\n");  break;
        case SCHED_DEADLINE: printf("SCHED_DEADLINE\n"); break;
        default: printf("Unknown (%d)\n", policy);
    }
}

int main() {
    struct sched_attr attr;

    memset(&attr, 0, sizeof(attr));
    attr.size = sizeof(attr);
    attr.sched_policy = SCHED_RR;   

    if (attr.sched_policy == SCHED_DEADLINE) {
        attr.sched_runtime  = 10 * 1000 * 1000;  // 10ms in ns
        attr.sched_deadline = 30 * 1000 * 1000;  // 30ms in ns
        attr.sched_period   = 30 * 1000 * 1000;  // 30ms in ns
    } else if (attr.sched_policy == SCHED_FIFO || attr.sched_policy == SCHED_RR)
          attr.sched_priority = 10;

    if (syscall(SYS_sched_setattr, 0, &attr, 0) < 0) {
        perror("sched_setattr");
        printf("You probably need to run as root.\n");
        return 1;
    }

    printf("Policy successfully changed.\n");
    memset(&attr, 0, sizeof(attr));
    attr.size = sizeof(attr);
    if (syscall(SYS_sched_getattr, 0, &attr, sizeof(attr), 0) < 0) {
        perror("sched_getattr");
        return 1;
    }

    printf("=== sched_getattr data ===\n");
    printf("Size: %u\n", attr.size);
    printf("Policy: ");
    print_policy(attr.sched_policy);
    printf("Flags: %llu\n", (unsigned long long)attr.sched_flags);
    printf("Nice value: %d\n", attr.sched_nice);
    printf("Priority: %u\n", attr.sched_priority);
    printf("Runtime: %llu\n", (unsigned long long)attr.sched_runtime);
    printf("Deadline: %llu\n", (unsigned long long)attr.sched_deadline);
    printf("Period: %llu\n", (unsigned long long)attr.sched_period);
    return 0;
}
