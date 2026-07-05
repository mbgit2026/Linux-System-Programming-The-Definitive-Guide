#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int           fd;
    unsigned int  seals;

    if (argc != 2) {
        fprintf(stderr, "%s /proc/PID/fd/FD\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1)
        err(EXIT_FAILURE, "open");

    seals = fcntl(fd, F_GET_SEALS);
    if (seals == -1)
        err(EXIT_FAILURE, "fcntl");

    printf("Existing seals:");
    if (seals & F_SEAL_SEAL)
        printf(" SEAL");
    if (seals & F_SEAL_GROW)
        printf(" GROW");
    if (seals & F_SEAL_WRITE)
        printf(" WRITE");
    if (seals & F_SEAL_FUTURE_WRITE)
        printf(" FUTURE_WRITE");
    if (seals & F_SEAL_SHRINK)
        printf(" SHRINK");
    printf("\n");

    char *buff = mmap(NULL, 4096, PROT_READ , MAP_SHARED, fd, 0);
    if (buff == MAP_FAILED) 
        err(EXIT_FAILURE, "mmap");
        
    printf("BUFFER ELEMENT: %c", buff[129]);
    exit(EXIT_SUCCESS);
}
