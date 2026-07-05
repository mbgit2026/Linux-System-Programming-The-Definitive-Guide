#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(void)
{
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Get current pipe capacity */
    int size = fcntl(pipefd[0], F_GETPIPE_SZ);
    if (size == -1) {
        perror("F_GETPIPE_SZ");
        return 1;
    }

    printf("Original pipe capacity: %d bytes\n", size);

    /* Try to set a new pipe capacity */
    int new_size = size * 2;
    int ret = fcntl(pipefd[0], F_SETPIPE_SZ, new_size);
    if (ret == -1) {
        perror("F_SETPIPE_SZ");
        printf("Note: Increasing pipe size may require CAP_SYS_RESOURCE\n");
    } else {
        printf("Requested new pipe capacity: %d bytes\n", new_size);
    }

    /* Get pipe capacity again */
    size = fcntl(pipefd[0], F_GETPIPE_SZ);
    if (size == -1) {
        perror("F_GETPIPE_SZ");
        return 1;
    }

    printf("Current pipe capacity: %d bytes\n", size);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
