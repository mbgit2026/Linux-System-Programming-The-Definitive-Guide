#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(void)
{
    pid_t child;
    int pidfd;
    struct pollfd pfd;
    int status;

    child = fork();
    if (child == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child == 0) {
        /* Child */
        printf("Child PID: %d\n", getpid());
        sleep(200);
        _exit(0);
    }

    /* Parent */
    pidfd = syscall(SYS_pidfd_open, child, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    pfd.fd = pidfd;
    pfd.events = POLLIN;

    printf("Parent waiting for child to exit...\n");

    if (poll(&pfd, 1, -1) == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    if (pfd.revents & POLLIN) {
        printf("pidfd is readable: child has exited\n");
    }
    getchar();

    /* Reap the child */
    if (waitpid(child, &status, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status))
        printf("Child exit status: %d\n", WEXITSTATUS(status));

    if (WIFSIGNALED(status))
        printf("Child killed by signal %d (%s)\n", WTERMSIG(status), strsignal(WTERMSIG(status)));

    close(pidfd);
    return 0;
}
