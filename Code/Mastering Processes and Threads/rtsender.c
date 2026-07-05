#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int i;
    union sigval val;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);

    for (i = 0; i < 5; i++) {
        val.sival_int = i;

        if (sigqueue(pid, SIGRTMIN, val) == -1) {
            perror("sigqueue");
            exit(EXIT_FAILURE);
        }

        printf("Sent value %d\n", i);
    }

    return 0;
}