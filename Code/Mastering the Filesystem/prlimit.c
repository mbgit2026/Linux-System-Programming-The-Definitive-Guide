#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    struct rlimit old_limit, new_limit;

    pid_t pid = 0; 

    if (prlimit(pid, RLIMIT_NOFILE, NULL, &old_limit) == -1) {
        perror("prlimit (get)");
        return 1;
    }

    printf("Current RLIMIT_NOFILE:\n");
    printf("  Soft limit: %llu\n", (unsigned long long) old_limit.rlim_cur);
    printf("  Hard limit: %llu\n", (unsigned long long) old_limit.rlim_max);

    new_limit.rlim_cur = 64;
    new_limit.rlim_max = old_limit.rlim_max; // keep the hard limit unchanged

    if (prlimit(pid, RLIMIT_NOFILE, &new_limit, NULL) == -1) {
        perror("prlimit (set)");
        return 1;
    }

    printf("New soft limit set to 64.\n");
    struct rlimit verify_limit;
    if (prlimit(pid, RLIMIT_NOFILE, NULL, &verify_limit) == -1) {
        perror("prlimit (verify)");
        return 1;
    }

    printf("Verified RLIMIT_NOFILE:\n");
    printf("  Soft limit: %llu\n", (unsigned long long) verify_limit.rlim_cur);
    printf("  Hard limit: %llu\n", (unsigned long long) verify_limit.rlim_max);

    return 0;
}
