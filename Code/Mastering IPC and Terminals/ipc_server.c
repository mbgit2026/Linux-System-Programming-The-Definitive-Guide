#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_PATH "ipc_socket"

int main(void)
{
    int sock, client;
    struct sockaddr_un addr;
    socklen_t addrlen;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Enable credential reception */
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    /* named socket */
    strcpy(addr.sun_path, SOCKET_PATH);
    unlink(SOCKET_PATH);
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(addr.sun_path);
       
    /* abstract socket */
    /*
    addr.sun_path[0] = '\0'; 
    strcpy(addr.sun_path + 1, "ipc_socket");
    addrlen = offsetof(struct sockaddr_un, sun_path) + 1 + strlen("ipc_socket");
    */

    if (bind(sock, (struct sockaddr *)&addr, addrlen) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server: waiting for connection...\n");

    client = accept(sock, NULL, NULL);
    if (client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    /* Receive message + credentials */
    struct msghdr msg = {0};
    struct iovec iov;
    char buf[16];

    char control[CMSG_SPACE(sizeof(struct ucred))];
    struct cmsghdr *cmsg;

    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    if (recvmsg(client, &msg, 0) < 0) {
        perror("recvmsg");
        exit(EXIT_FAILURE);
    }

    printf("Server: received message: %s\n", buf);

    /* Extract credentials */
    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && cmsg->cmsg_type == SCM_CREDENTIALS) {
        struct ucred *cred = (struct ucred *)CMSG_DATA(cmsg);
        printf("Client PID=%d UID=%d GID=%d\n", cred->pid, cred->uid, cred->gid);
    }

    /* Open file and send FD */
    int fd = open("shared.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    write(fd, "Created by server\n", 18);

    struct msghdr smsg = {0};
    struct iovec siov;
    char sbuf = 'F';

    char scontrol[CMSG_SPACE(sizeof(int))];
    struct cmsghdr *scmsg;

    siov.iov_base = &sbuf;
    siov.iov_len = sizeof(sbuf);

    smsg.msg_iov = &siov;
    smsg.msg_iovlen = 1;
    smsg.msg_control = scontrol;
    smsg.msg_controllen = sizeof(scontrol);

    scmsg = CMSG_FIRSTHDR(&smsg);
    scmsg->cmsg_level = SOL_SOCKET;
    scmsg->cmsg_type  = SCM_RIGHTS;
    scmsg->cmsg_len   = CMSG_LEN(sizeof(int));

    *(int *)CMSG_DATA(scmsg) = fd;

    if (sendmsg(client, &smsg, 0) < 0) {
        perror("sendmsg");
        exit(EXIT_FAILURE);
    }

    printf("Server: sent file descriptor\n");

    close(fd);
    close(client);
    close(sock);

    return 0;
}