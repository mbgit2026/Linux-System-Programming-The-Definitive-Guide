#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
    char *addr;
    int fd;
    struct stat file;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    if (fstat(fd, &file) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    addr = mmap(NULL, file.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    if (write(STDOUT_FILENO, addr, file.st_size) != file.st_size) {
        perror("write");
        munmap(addr, file.st_size);
        close(fd);
        return EXIT_FAILURE;
    }

    if (munmap(addr, file.st_size) == -1)
        perror("munmap");

    close(fd);

    return EXIT_SUCCESS;
}
