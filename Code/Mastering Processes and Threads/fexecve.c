#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd = open("/bin/ls", O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char *argv[] = { "ls", "-l", "/", NULL };
    char *envp[] = { "PATH=/bin:/usr/bin", NULL };

    if (fexecve(fd, argv, envp) == -1) {
        perror("fexecve");
        exit(EXIT_FAILURE);
    }

    return 0;
}
