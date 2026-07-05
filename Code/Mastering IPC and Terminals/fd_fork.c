#define _GNU_SOURCE
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void print_fds(const char *msg)
{
    printf("%s\n", msg);
    for (int fd = 0; fd < 10; fd++) {
        if (fcntl(fd, F_GETFD) != -1) {
            printf("  fd %d is open\n", fd);
        }
    }
}

int main(void)
{
    int sp[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* -------- child -------- */
        close(sp[0]);
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

        print_fds("Child: before recvmsg()");

        if (recvmsg(sp[1], &msg, 0) == -1) {
            perror("recvmsg");
            exit(EXIT_FAILURE);
        }

        cmsg = CMSG_FIRSTHDR(&msg);
        int fd = *(int *)CMSG_DATA(cmsg);

        printf("Child: received FD %d\n", fd);
        print_fds("Child: after recvmsg()");
        write(fd, "Written by child\n", 17);
        close(fd);
        close(sp[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        /* -------- parent -------- */
        close(sp[1]);

        /* open AFTER fork */
        int fd = open("after_fork.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }

        write(fd, "Opened after fork\n", 18);

        struct msghdr msg = {0};
        struct iovec iov;
        char buf = 'A';
        char control[CMSG_SPACE(sizeof(int))];
        struct cmsghdr *cmsg;

        iov.iov_base = &buf;
        iov.iov_len = sizeof(buf);

        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_control = control;
        msg.msg_controllen = sizeof(control);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type  = SCM_RIGHTS;
        cmsg->cmsg_len   = CMSG_LEN(sizeof(int));

        *(int *)CMSG_DATA(cmsg) = fd;

        if (sendmsg(sp[0], &msg, 0) == -1) {
            perror("sendmsg");
            exit(EXIT_FAILURE);
        }

        printf("Parent: sent FD %d\n", fd);

        close(fd);
        close(sp[0]);
    }

    return 0;
}