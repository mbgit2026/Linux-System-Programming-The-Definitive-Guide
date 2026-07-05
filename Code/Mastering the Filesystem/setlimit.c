#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit");
        return 1;
    }

    printf("Current limits for RLIMIT_NOFILE:\n");
    printf("  Soft limit: %lu\n", limit.rlim_cur);
    printf("  Hard limit: %lu\n", limit.rlim_max);

    limit.rlim_cur = 64;

    if (setrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("setrlimit");
        return 1;
    }

    printf("\nNew soft limit set to 64.\n");


    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit");
        return 1;
    }

    printf("Updated limits for RLIMIT_NOFILE:\n");
    printf("  Soft limit: %lu\n", limit.rlim_cur);
    printf("  Hard limit: %lu\n", limit.rlim_max);

    return 0;
}
