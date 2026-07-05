#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

// Set socket to non-blocking
int set_nonblocking(int fd) {
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

int main() {
    int server_fd, epoll_fd;
    struct sockaddr_in address;
    struct epoll_event ev, events[MAX_EVENTS];

    char buffer[BUFFER_SIZE];

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 10);

    set_nonblocking(server_fd);

    // Create epoll instance
    epoll_fd = epoll_create1(0);

    // Add server socket to epoll
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    printf("Epoll server listening on port %d...\n", PORT);

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; i++) {

            // New connection
            if (events[i].data.fd == server_fd) {
                int client_fd;
                while ((client_fd = accept(server_fd, NULL, NULL)) != -1) {                
                    set_nonblocking(client_fd);
                    ev.events = EPOLLIN;
                    ev.data.fd = client_fd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);
                    printf("New client: %d\n", client_fd);
                } 
                if (errno != EAGAIN && errno != EWOULDBLOCK) {
                    perror("accept");
                }
            }
            else {
                int client_fd = events[i].data.fd;
                int count = recv(client_fd, buffer, BUFFER_SIZE, 0);

                if (count <= 0) {
                    if (count < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                        // No data available right now → not an error
                        continue;
                    }

                    // Remove from epoll (good practice)
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    // Client closed or error
                    close(client_fd);
                    printf("Client disconnected: %d\n", client_fd);
                } else {
                    buffer[count] = '\0';
                    printf("Client %d: %s\n", client_fd, buffer);

                    // Echo back
                    send(client_fd, buffer, count, 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}