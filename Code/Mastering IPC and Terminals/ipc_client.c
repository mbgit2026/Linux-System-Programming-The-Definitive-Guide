#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "ipc_socket"

int main(void)
{
    int sock;
    struct sockaddr_un addr;
    socklen_t addrlen;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    
    /* named socket */
    strcpy(addr.sun_path, SOCKET_PATH);
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path);
    
    /* abstracct socket */
    /*
    addr.sun_path[0] = '\0'; 
    strcpy(addr.sun_path + 1, "ipc_socket");
    addrlen = offsetof(struct sockaddr_un, sun_path) + 1 + strlen("ipc_socket");
    */

    if (connect(sock, (struct sockaddr *)&addr, addrlen) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* Send message (kernel will attach credentials) */
    write(sock, "Hello server", 13);

    /* Receive FD */
    struct msghdr msg = {0};
    struct iovec iov;
    char buf;

    char control[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cmsg;

    iov.iov_base = &buf;
    iov.iov_len = sizeof(buf);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    if (recvmsg(sock, &msg, 0) < 0) {
        perror("recvmsg");
        exit(EXIT_FAILURE);
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    int fd = *(int *)CMSG_DATA(cmsg);
    printf("Client: received file descriptor %d\n", fd);
    write(fd, "Written by client\n", 18);

    close(fd);
    close(sock);
    return 0;
}