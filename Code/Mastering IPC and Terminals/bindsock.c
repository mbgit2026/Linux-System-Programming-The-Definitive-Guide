#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#define MY_SOCK_PATH "/somepath"
#define LISTEN_BACKLOG 50

int main(void)
{
    int                 sfd, cfd;
    socklen_t           peer_addr_size;
    struct sockaddr_un  my_addr, peer_addr;

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        err(EXIT_FAILURE, "socket");

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_UNIX;
    strncpy(my_addr.sun_path, MY_SOCK_PATH, sizeof(my_addr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &my_addr, sizeof(my_addr)) == -1)
        err(EXIT_FAILURE, "bind");

    if (listen(sfd, LISTEN_BACKLOG) == -1)
        err(EXIT_FAILURE, "listen");

    /* Now we can accept incoming connections one at a time using accept(2).  */

    peer_addr_size = sizeof(peer_addr);
    cfd = accept(sfd, (struct sockaddr *) &peer_addr, &peer_addr_size);
    if (cfd == -1)
        err(EXIT_FAILURE, "accept");
  
        /* Send a message to the client */
    const char *msg = "Hello from server";
    if (write(cfd, msg, strlen(msg)) == -1)
        err(EXIT_FAILURE, "write");

    char buf[128];
    ssize_t n = read(cfd, buf, sizeof(buf) - 1);
    if (n == -1)
        err(EXIT_FAILURE, "read");
    buf[n] = '\0';
    printf("\nFrom client: %s", buf);


    if (close(sfd) == -1)
        err(EXIT_FAILURE, "close");

    if (unlink(MY_SOCK_PATH) == -1)
        err(EXIT_FAILURE, "unlink");
}
