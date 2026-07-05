#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int fd, size;
    struct sockaddr_un un;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    un.sun_family = AF_UNIX;
    strncpy(un.sun_path, "foo.socket", sizeof(un.sun_path) - 1);
    un.sun_path[sizeof(un.sun_path) - 1] = '\0';

    unlink("foo.socket");  // remove existing file

    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    if (bind(fd, (struct sockaddr *)&un, size) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("UNIX domain socket bound to %s\n", un.sun_path);

    listen(fd, 5);
    int client_fd = accept(fd, NULL, NULL);
    char buf[100];
    read(client_fd, buf, sizeof(buf));
    printf("Received: %s\n", buf);
    close(client_fd);

    close(fd);
    return 0;
}