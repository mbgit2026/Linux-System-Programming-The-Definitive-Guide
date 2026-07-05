#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    sigset_t set;
    siginfo_t info;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL);

    printf("PID: %d\n", getpid());
    printf("Press Ctrl-C...\n");

    /* Wait synchronously for SIGINT */
    sigwaitinfo(&set, &info);
    printf("Received SIGINT from PID %d\n", info.si_pid);
    return 0;
}