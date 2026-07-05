#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

void sigio_handler(int sig) {
    (void)sig;
    printf("[lease holder] Received SIGIO: lease break requested!\n");
}

int main(void) {
    int fd;

    // Open a test file
    fd = open("file.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Set up signal handler for SIGIO
    struct sigaction sa = {0};
    sa.sa_handler = sigio_handler;
    sigaction(SIGIO, &sa, NULL);

    // Tell kernel which process to notify
    if (fcntl(fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Enable asynchronous I/O signals
    if (fcntl(fd, F_SETFL, O_ASYNC) == -1) {
        perror("fcntl F_SETFL O_ASYNC");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Acquire a write lease
    if (fcntl(fd, F_SETLEASE, F_WRLCK) == -1) {
        perror("fcntl F_SETLEASE");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("[lease holder] Write lease acquired on file.\n");

    // Query lease type
    int lease = fcntl(fd, F_GETLEASE);
    if (lease == -1) {
        perror("fcntl F_GETLEASE");
    } else {
        printf("[lease holder] Current lease type: %s\n",
               (lease == F_WRLCK) ? "WRITE lease" :
               (lease == F_RDLCK) ? "READ lease" :
               (lease == F_UNLCK) ? "NO lease" : "UNKNOWN");
    }

    printf("[lease holder] Now waiting. Try in another terminal:\n");
    printf("   echo test >> leasefile.txt\n");
    printf("or just open leasefile.txt with another program.\n");
    printf("You should see SIGIO when the lease is broken.\n");

    // Wait so we can receive signals
    for (;;) pause();

    close(fd);
    return 0;
}
