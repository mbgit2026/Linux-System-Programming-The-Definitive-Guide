#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(void)
{
    sigset_t set;
    siginfo_t info;
    int i, sig;

    sigemptyset(&set);
    sigaddset(&set, SIGRTMIN);

    /* Block SIGRTMIN so it becomes pending */
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("PID: %d\n", getpid());
    printf("Send several SIGRTMIN signals...\n");

    /* Receive 5 signals */
    for (i = 0; i < 5; i++) {

        sig = sigwaitinfo(&set, &info);
        if (sig == -1) {
            perror("sigwaitinfo");
            exit(EXIT_FAILURE);
        }

        printf("Received signal %d with value %d from PID %d\n",
               sig,
               info.si_value.sival_int,
               info.si_pid);
    }

    return 0;
}