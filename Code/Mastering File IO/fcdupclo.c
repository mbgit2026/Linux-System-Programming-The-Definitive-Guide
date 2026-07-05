#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

extern char **environ;

int main() {
    int fd, newfd;
    pid_t pid;

    // Open a file
    fd = open("dupfd_cloexec_demo.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Duplicate fd with CLOEXEC
    newfd = fcntl(fd, F_DUPFD_CLOEXEC, 10);
    if (newfd == -1) {
        perror("fcntl");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("[Parent] Original FD: %d, New FD (CLOEXEC): %d\n", fd, newfd);

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child
        char pid_str[32];
        snprintf(pid_str, sizeof(pid_str), "%d", getpid());

        char *args[] = {"./showfds", pid_str, NULL};
        printf("[Child] Executing ./showfds %s\n", pid_str);
        execve("./showfds", args, environ);
        perror("execve");
        _exit(1);
    } else {
        //wait(NULL);
        if (fcntl(newfd, F_SETFD, FD_CLOEXEC) == -1) 
            perror("fcntl");
    }

    close(fd);
    close(newfd);
    return 0;
}

