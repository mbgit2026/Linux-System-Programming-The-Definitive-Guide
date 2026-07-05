#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        printf("[Child] PID: %d\n", getpid());

        struct rlimit limit;
        if (prlimit(0, RLIMIT_NOFILE, NULL, &limit) == -1) {
            perror("[Child] prlimit (get)");
            exit(1);
        }

        printf("[Child] RLIMIT_NOFILE:\n");
        printf("  Soft: %llu\n", (unsigned long long)limit.rlim_cur);
        printf("  Hard: %llu\n", (unsigned long long)limit.rlim_max);

        // Sleep to keep process alive so parent can change its limits
        sleep(2);

        // Get updated limits after parent modifies them
        if (prlimit(0, RLIMIT_NOFILE, NULL, &limit) == -1) {
            perror("[Child] prlimit (verify)");
            exit(1);
        }

        printf("[Child] Updated RLIMIT_NOFILE:\n");
        printf("  Soft: %llu\n", (unsigned long long)limit.rlim_cur);
        printf("  Hard: %llu\n", (unsigned long long)limit.rlim_max);

        return 0;
    } else {
        // --- PARENT PROCESS ---
        sleep(1);  // Give child time to start and sleep

        printf("[Parent] Changing child (%d) RLIMIT_NOFILE to soft=64...\n", pid);

        struct rlimit new_limit;
        new_limit.rlim_cur = 64;
        new_limit.rlim_max = 64;

        if (prlimit(pid, RLIMIT_NOFILE, &new_limit, NULL) == -1) {
            perror("[Parent] prlimit (set)");
            return 1;
        }

        printf("[Parent] Limit change requested. Waiting for child...\n");

        wait(NULL);
        printf("[Parent] Child finished.\n");
        return 0;
    }
}
