#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/socket.h>

#define MSG1 "hello"
#define MSG2 "Hi!"

int fd_pipe(int fd[2])
{
    return(socketpair(AF_UNIX, SOCK_STREAM, 0, fd));
}

int main() {

    int sp[2];
    char buf[6];

    if (fd_pipe(sp) == -1) {
        perror("socketpair");
        exit(EXIT_FAILURE);
    }
    
    printf("sp[0] = %d\n", sp[0]);
    printf("sp[1] = %d\n", sp[1]);

    /* Write from sp[0] and read from sp[1] */

    write(sp[0], MSG1, sizeof(MSG1) -1);
    read(sp[1], buf, sizeof(MSG1) -1);
    buf[sizeof(MSG1) -1] = '\0';
    printf("Received: %s\n", buf);

    write(sp[1], MSG2, sizeof(MSG2) -1);
    read(sp[0], buf, sizeof(MSG2) -1);
    buf[sizeof(MSG2) -1] = '\0';
    printf("Received: %s\n", buf);

    close(sp[0]);
    close(sp[1]);

    return 0;
}