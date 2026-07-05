#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("file.txt", O_RDONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Original fd = %d\n", fd);

    // Duplicate fd to newfd=10, with O_CLOEXEC
    int newfd = dup3(fd, 10, O_CLOEXEC);
    if (newfd == -1) {
        perror("dup3");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Duplicated fd = %d (with O_CLOEXEC)\n", newfd);

    // Prepare arguments for execve
    char *args[] = {"./openfd", NULL};
    char *envp[] = {NULL};

    printf("Now calling execve ./openfd\n");

    // execve replaces the process
    if (execve("./openfd", args, envp) == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }

    return 0; // never reached
}
