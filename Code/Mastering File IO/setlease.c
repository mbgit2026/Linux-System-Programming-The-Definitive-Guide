#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void handler(int sig) {
    if (sig == SIGIO) {
        printf("Lease broken! Another process tried to access the file.\n");
    }
}

int main() {
    const char *fname = "leased_file.txt";
    int fd;

    fd = open(fname, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fd, F_SETLEASE, F_WRLCK) == -1) {
        perror("fcntl F_SETLEASE");
        exit(EXIT_FAILURE);
    }

    printf("Lease set on %s.\n", fname);
    printf("Waiting for signals...\n");

    for (;;) {
        pause();
    }

    close(fd);
    return 0;
}
