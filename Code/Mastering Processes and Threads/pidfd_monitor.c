#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int            pidfd, ready;
    struct pollfd  pollfd;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    pidfd = syscall(SYS_pidfd_open, atoi(argv[1]), 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    pollfd.fd = pidfd;
    pollfd.events = POLLIN;

    ready = poll(&pollfd, 1, -1);
    if (ready == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    printf("Events (%#x): POLLIN is %sset\n", pollfd.revents,
            (pollfd.revents & POLLIN) ? "" : "not ");

    close(pidfd);
    exit(EXIT_SUCCESS);
}
