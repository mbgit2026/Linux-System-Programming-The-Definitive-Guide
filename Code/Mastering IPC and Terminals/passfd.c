#define _GNU_SOURCE
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int sp[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    /* Create a file to pass */
    int fd = open("shared.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    write(fd, "Hello via FD passing\n", 22);

    /* -------- sender side (sp[0]) -------- */
    struct msghdr msg;
    struct iovec iov;
    char buf = 'X';

    char control[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *cmsg;

    memset(&msg, 0, sizeof(msg));
    memset(control, 0, sizeof(control));

    iov.iov_base = &buf;
    iov.iov_len = sizeof(buf);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    /* attach file descriptor */
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type  = SCM_RIGHTS;
    cmsg->cmsg_len   = CMSG_LEN(sizeof(int));

    *(int *)CMSG_DATA(cmsg) = fd;

    if (sendmsg(sp[0], &msg, 0) == -1) {
        perror("sendmsg");
        exit(EXIT_FAILURE);
    }

    printf("Sender: sent file descriptor %d\n", fd);

    /* -------- receiver side (sp[1]) -------- */
    char recv_buf;
    char recv_control[CMSG_SPACE(sizeof(int))];

    struct msghdr rmsg;
    struct iovec riov;

    memset(&rmsg, 0, sizeof(rmsg));
    memset(recv_control, 0, sizeof(recv_control));

    riov.iov_base = &recv_buf;
    riov.iov_len = sizeof(recv_buf);

    rmsg.msg_iov = &riov;
    rmsg.msg_iovlen = 1;
    rmsg.msg_control = recv_control;
    rmsg.msg_controllen = sizeof(recv_control);

    if (recvmsg(sp[1], &rmsg, 0) == -1) {
        perror("recvmsg");
        exit(EXIT_FAILURE);
    }

    struct cmsghdr *rcmsg = CMSG_FIRSTHDR(&rmsg);
    int received_fd = *(int *)CMSG_DATA(rcmsg);

    printf("Receiver: got file descriptor %d\n", received_fd);

    /* Use received file descriptor */
    write(received_fd, "Written via received FD\n", 25);

    close(fd);
    close(received_fd);
    close(sp[0]);
    close(sp[1]);

    return 0;
}