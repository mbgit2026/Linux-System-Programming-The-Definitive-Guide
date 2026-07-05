
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid;
    siginfo_t info;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        sleep(20);
        _exit(42);
    }

    printf("Parent waiting for child...\n");

    if (waitid(P_PID, pid, &info, WEXITED) == -1) {
        perror("waitid");
        exit(EXIT_FAILURE);
    }

    if (info.si_code == CLD_EXITED) {
        printf("Child exited normally\n");
        printf("Exit status: %d\n", info.si_status);
    } else if (info.si_code == CLD_KILLED) {
        printf("Child killed by signal %d\n", info.si_status);
    }

    return 0;
}
