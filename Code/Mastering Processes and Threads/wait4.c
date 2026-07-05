#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    pid_t pid;
    int status;
    struct rusage usage;

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child PID: %d\n", getpid());
        sleep(1);         
        _exit(3);
    }

    pid_t w = wait4(pid, &status, 0, &usage);
    if (w == -1) {
        perror("wait4");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status)) {
        printf("Child exited normally\n");
        printf("Exit status: %d\n", WEXITSTATUS(status));
    }

    printf("User CPU time:   %ld.%06ld sec\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
    printf("System CPU time: %ld.%06ld sec\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);

    return 0;
}
