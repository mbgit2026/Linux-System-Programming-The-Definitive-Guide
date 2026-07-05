#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SOCKET_NAME "open_socket"

int main(void)
{
    int sock, client;
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* enable credentials */
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

    /* abstract socket */
    addr.sun_path[0] = '\0';
    strcpy(addr.sun_path + 1, SOCKET_NAME);

    socklen_t addrlen =
        offsetof(struct sockaddr_un, sun_path) + 1 + strlen(SOCKET_NAME);

    if (bind(sock, (struct sockaddr *)&addr, addrlen) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Secure open server (abstract socket) running...\n");
    client = accept(sock, NULL, NULL);
    if (client < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    /* ===== RECEIVE REQUEST + CREDENTIALS ===== */
    char filename[128];
    struct iovec iov;
    struct msghdr msg;
    char control[CMSG_SPACE(sizeof(struct ucred))];

    memset(&msg, 0, sizeof(msg));
    memset(control, 0, sizeof(control));

    iov.iov_base = filename;
    iov.iov_len = sizeof(filename) - 1;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);

    int n = recvmsg(client, &msg, 0);
    if (n <= 0) {
        perror("recvmsg");
        exit(EXIT_FAILURE);
    }

    filename[n] = '\0';
    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    struct ucred *cred = NULL;

    if (cmsg && cmsg->cmsg_type == SCM_CREDENTIALS) {
        cred = (struct ucred *)CMSG_DATA(cmsg);
        printf("Client UID=%d PID=%d\n", cred->uid, cred->pid);
    }

    if (!cred || cred->uid != 1000) {
        printf("Access denied\n");
        close(client);
        close(sock);
        return 0;
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* ===== SEND FD ===== */
    struct msghdr smsg = {0};
    struct iovec siov;
    char sbuf = 'F';
 
    char scontrol[CMSG_SPACE(sizeof(int))];
    memset(scontrol, 0, sizeof(scontrol));   

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

    close(fd);
    close(client);
    close(sock);
    return 0;
}