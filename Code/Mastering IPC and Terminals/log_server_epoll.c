#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "log.sock"
#define MAX_EVENTS  10

int main(void)
{
    int sock, epfd;
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(sock, (struct sockaddr *)&addr,
             offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev, events[MAX_EVENTS];

    ev.events = EPOLLIN;
    ev.data.fd = sock;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev) < 0) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    printf("Logger (epoll) running...\n");

    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n < 0) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < n; i++) {

            if (events[i].data.fd == sock) {
                /* new client */
                int client = accept(sock, NULL, NULL);
                if (client < 0) {
                    perror("accept");
                    continue;
                }

                ev.events = EPOLLIN;
                ev.data.fd = client;

                epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
            }
            else {
                /* client sent data */
                char buf[128];
                int fd = events[i].data.fd;

                int nread = read(fd, buf, sizeof(buf) - 1);
                if (nread <= 0) {
                    close(fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                } else {
                    buf[nread] = '\0';
                    printf("Log: %s\n", buf);
                }
            }
        }
    }
}