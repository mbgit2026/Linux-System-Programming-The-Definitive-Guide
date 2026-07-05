#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "log_dgram.sock"

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_un addr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    /* no connect() needed (but optional) */
    sendto(sock, argv[1], strlen(argv[1]), 0, (struct sockaddr *)&addr, sizeof(addr));
    sendto(sock, "Data", 4, 0, (struct sockaddr *)&addr, sizeof(addr));


    printf("Message sent\n");

    close(sock);
    return 0;
}