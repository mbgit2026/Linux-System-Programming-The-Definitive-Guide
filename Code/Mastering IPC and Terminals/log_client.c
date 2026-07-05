#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "log.sock"

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_un addr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s message\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr *)&addr, offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    write(sock, argv[1], strlen(argv[1]));
    //write(sock, "data", 4);

    
    close(sock);
    return 0;
}