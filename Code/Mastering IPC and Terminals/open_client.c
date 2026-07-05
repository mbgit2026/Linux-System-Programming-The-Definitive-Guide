#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_NAME "open_socket"

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_un addr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    /* abstract socket */
    addr.sun_path[0] = '\0';
    strcpy(addr.sun_path + 1, SOCKET_NAME);

    socklen_t addrlen =
        offsetof(struct sockaddr_un, sun_path) + 1 + strlen(SOCKET_NAME);

    if (connect(sock, (struct sockaddr *)&addr, addrlen) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    /* send filename */
    write(sock, argv[1], strlen(argv[1]));

    /* ===== RECEIVE FD ===== */
    struct msghdr msg;
    struct iovec iov;
    char buf;

    char control[
        CMSG_SPACE(sizeof(struct ucred)) +
        CMSG_SPACE(sizeof(int))
    ];

    memset(&msg, 0, sizeof(msg));
    memset(control, 0, sizeof(control));

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

    struct cmsghdr *cmsg;
    int fd = -1;

    for (cmsg = CMSG_FIRSTHDR(&msg);
    cmsg != NULL;
    cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
            fd = *(int *)CMSG_DATA(cmsg);
        }
    }
    if (fd == -1) {
        fprintf(stderr, "No valid descriptor received\n");
        exit(EXIT_FAILURE);
    }

    printf("Received FD=%d\n", fd);

    /* ===== USE FILE ===== */
    char buffer[64];
    int n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, n);
    }

    close(fd);
    close(sock);
    return 0;
}