#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(void)
{
    int fd = open("/bin/echo", O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        perror("open src");
        exit(1);
    }

    int memfd = memfd_create("echo_mem", 0); 
    if (memfd < 0) {
        perror("memfd_create");
        exit(1);
    }

    char buf[4096];
    ssize_t n;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        if (write(memfd, buf, n) != n) {
            perror("write memfd");
            exit(1);
        }
    }

    close(fd);

    lseek(memfd, 0, SEEK_SET);

    char *argv[] = { "echo", "Hello from memfd", NULL };
    char *envp[] = { "PATH=/bin:/usr/bin", NULL };

    fexecve(memfd, argv, envp);

    perror("fexecve");
    exit(1);
}
