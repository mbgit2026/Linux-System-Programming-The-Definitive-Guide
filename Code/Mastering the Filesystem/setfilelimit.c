#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main() {
    struct rlimit limit;

    if (getrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("getrlimit");
        return 1;
    }

    printf("\nCurrent limit: %u\n", limit.rlim_cur);

    limit.rlim_cur = 5;

    if (setrlimit(RLIMIT_NOFILE, &limit) == -1) {
        perror("setrlimit");
        return 1;
    }

    printf("Set RLIMIT_NOFILE soft limit to 5.\n");

    int fds[4];
    for (int i = 0; i < 4; ++i) {
        char filename[32];
        snprintf(filename, sizeof(filename), "file%d.txt", i + 1);
        fds[i] = open(filename, O_CREAT | O_WRONLY, 0644);
        if (fds[i] == -1) {
            printf("open('%s') failed: %s\n", filename, strerror(errno));
        } else {
            printf("open('%s') succeeded: fd=%d\n", filename, fds[i]);
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (fds[i] != -1)
            close(fds[i]);
    }

    return 0;
}
