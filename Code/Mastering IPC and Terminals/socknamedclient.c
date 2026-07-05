#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "foo.socket"
#define MSG "Hello from client"

int main(void)
{
    int fd;
    struct sockaddr_un un;
    int size;

    /* Create socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Prepare address */
    un.sun_family = AF_UNIX;
    strncpy(un.sun_path, SOCKET_PATH, sizeof(un.sun_path) - 1);
    un.sun_path[sizeof(un.sun_path) - 1] = '\0';

    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    /* Connect to server */
    if (connect(fd, (struct sockaddr *)&un, size) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Send message */
    if (write(fd, MSG, sizeof(MSG) - 1) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("Message sent: %s\n", MSG);

    close(fd);
    return 0;
}