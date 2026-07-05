#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "log.sock"

int main(void)
{
    int sock, client;
    struct sockaddr_un addr;
    char buf[128];

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

    printf("Logger server listening...\n");

    while (1) {
        client = accept(sock, NULL, NULL);
        if (client < 0) {
            perror("accept");
            continue;
        }

        int n = read(client, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("Log: %s\n", buf);
        }

        close(client);
    }
}