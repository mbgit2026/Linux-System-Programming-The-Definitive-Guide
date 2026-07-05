#define _GNU_SOURCE
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
    int sp[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }

    /* close stdout BEFORE fork */
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        /* -------- child -------- */
        close(sp[0]);

        /* stdout is currently invalid */
        write(STDOUT_FILENO, "Child: try write on stdout\n", 26); // fails, no data go into restored.txt         

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

        if (recvmsg(sp[1], &msg, 0) == -1) {
            perror("recvmsg");
            exit(EXIT_FAILURE);
        } 

        cmsg = CMSG_FIRSTHDR(&msg);
        int fd = *(int *)CMSG_DATA(cmsg);


        if (fd != STDERR_FILENO) {
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        


        write(STDERR_FILENO, "Child: stderr restored via FD passing\n", 39);        
        close(sp[1]);
        exit(EXIT_SUCCESS);
    }
    else {
        /* -------- parent -------- */
        close(sp[1]);
        int fd = open("restored.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        
        dup2(fd, STDERR_FILENO);

        struct msghdr msg = {0};
        struct iovec iov;
        char buf = 'X';
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
            

        close(fd);
        close(sp[0]);
    }

    return 0;
}