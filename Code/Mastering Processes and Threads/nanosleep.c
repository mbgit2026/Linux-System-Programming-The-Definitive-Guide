#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

void handler(int sig) {
    printf("\nSignal %d received!\n", sig);
}

int main() {
    struct timespec req, rem;

    signal(SIGINT, handler);

    req.tv_sec = 5;
    req.tv_nsec = 500000000;

    printf("Sleeping for 5.5 seconds. Press Ctrl+C to interrupt.\n");

    if (nanosleep(&req, &rem) == -1 && errno == EINTR) {
        printf("Sleep interrupted!\n");
        printf("Remaining time: %ld sec %ld nsec\n", rem.tv_sec, rem.tv_nsec);
    } else {
        printf("Sleep finished normally.\n");
    }

    return 0;
}