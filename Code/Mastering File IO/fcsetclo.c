#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int flags = fcntl(STDIN_FILENO, F_GETFD);
    if (flags == -1) {
        perror("fcntl(F_GETFD)");
        exit(1);
    }

    if (fcntl(STDIN_FILENO, F_SETFD, flags | FD_CLOEXEC) == -1) {
        perror("fcntl(F_SETFD)");
        exit(1);
    }

    printf("FD_CLOEXEC set on stdin, now execve...\n");

    char *argv[] = {"./fcreadstdin", NULL};
    char *envp[] = {NULL};
    execve("./fcreadstdin", argv, envp);

    perror("execve");
    return 1;
}
