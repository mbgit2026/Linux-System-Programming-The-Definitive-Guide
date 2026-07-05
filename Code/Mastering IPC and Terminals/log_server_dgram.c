#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "log_dgram.sock"

int main(void)
{
    int sock;
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(sock, (struct sockaddr *)&addr,
             offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("DGRAM logger running (stateless)\n");

    while (1) {
        char buf[128], buf2[128];
        struct sockaddr_un client_addr;
        socklen_t len = sizeof(client_addr);
        

        int n = recvfrom(sock, buf, sizeof(buf) - 1, 0,
                         (struct sockaddr *)&client_addr, &len);


        if (n <= 0)
            continue;

        buf[n] = '\0';
        printf("Log: %s\n", buf);

    }
}