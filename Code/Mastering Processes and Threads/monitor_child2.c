#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

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
        printf("Child PID: %d\n", getpid());
        sleep(100);  
        _exit(0);
    }

    sleep(3);
    printf("\nPress enter to send a signal to child");
    getchar();

    pidfd = syscall(SYS_pidfd_open, child, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        exit(EXIT_FAILURE);
    }

    pfd.fd = pidfd;
    pfd.events = POLLIN;

    printf("Parent: sending SIGTERM via pidfd\n");
    if (syscall(SYS_pidfd_send_signal, pidfd, SIGTERM, NULL, 0) == -1) {
        perror("pidfd_send_signal");
        exit(EXIT_FAILURE);
    }

    printf("Parent waiting for child to exit...\n");
    if (poll(&pfd, 1, -1) == -1) {
        perror("poll");
        exit(EXIT_FAILURE);
    }

    if (pfd.revents & POLLIN) 
        printf("pidfd is readable: child has exited (zombie)\n");

    printf("\nPress enter to reap the child");
    getchar();
    if (waitpid(child, &status, 0) == -1) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    if (WIFEXITED(status))
        printf("Child exited normally: %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        printf("Child killed by signal %d (%s)\n",
               WTERMSIG(status),
               strsignal(WTERMSIG(status)));
    close(pidfd);
    return 0;
}
