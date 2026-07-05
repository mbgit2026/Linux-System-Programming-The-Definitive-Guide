#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MY_SOCK_PATH "/somepath"

int main(void)
{
    int sfd;
    struct sockaddr_un addr;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        err(EXIT_FAILURE, "socket");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, MY_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        err(EXIT_FAILURE, "connect");

    const char *msg = "Hello from client";
    if (write(sfd, msg, strlen(msg)) == -1)
        err(EXIT_FAILURE, "write");

    char buf[100];
    ssize_t n = read(sfd, buf, sizeof(buf) - 1);
    if (n == -1)
        err(EXIT_FAILURE, "read");

    buf[n] = '\0';
    write(STDOUT_FILENO, buf, n);

    close(sfd);
}