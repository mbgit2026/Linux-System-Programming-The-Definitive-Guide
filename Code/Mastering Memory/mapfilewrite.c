#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *addr;
    int fd;
    size_t len;
    char buffer[1024];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("Enter data:\n");
    if (!fgets(buffer, sizeof(buffer), stdin)) {
        perror("fgets");
        close(fd);
        return EXIT_FAILURE;
    }

    len = strlen(buffer);

    /* Resize file */
    if (ftruncate(fd, len) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    /* Map file */
    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    memcpy(addr, buffer, len);

    if (msync(addr, len, MS_SYNC) == -1)
        perror("msync");

    if (munmap(addr, len) == -1)
        perror("munmap");

    close(fd);

    /* Print file contents */
    write(STDOUT_FILENO, buffer, len);

    return EXIT_SUCCESS;
}
