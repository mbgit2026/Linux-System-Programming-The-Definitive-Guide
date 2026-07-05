define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int pipefd[2];
    struct epoll_event ev;
    int epfd;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    epfd = epoll_create1(0);
    if (epfd == -1) {
        perror("epoll_create1");
        return 1;
    }

    // Monitor read end with EPOLLEXCLUSIVE
    ev.events = EPOLLIN | EPOLLEXCLUSIVE;
    ev.data.fd = pipefd[0];

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipefd[0], &ev) == -1) {
        perror("epoll_ctl");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Child worker
        while (1) {
            struct epoll_event e;
            epoll_wait(epfd, &e, 1, -1);

            char buf[64];
            int n = read(pipefd[0], buf, sizeof(buf)-1);
            if (n > 0) {
                buf[n] = '\0';
                printf("[CHILD] handled: %s\n", buf);
            }
        }
    } else {
        // Parent worker
        while (1) {
            struct epoll_event e;
            epoll_wait(epfd, &e, 1, -1);

            char buf[64];
            int n = read(pipefd[0], buf, sizeof(buf)-1);
            if (n > 0) {
                buf[n] = '\0';
                printf("[PARENT] handled: %s\n", buf);
            }
        }
    }
}

