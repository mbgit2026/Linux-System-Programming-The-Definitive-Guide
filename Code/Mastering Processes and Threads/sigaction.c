#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig)
{
    printf("Received signal %d (%s)\n", sig, strsignal(sig));
    fflush(stdout);
}

int main(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send SIGINT (Ctrl+C) to test...\n");
    while (1) 
        pause(); 
    return 0;
}