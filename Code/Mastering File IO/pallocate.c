//#define _XOPEN_SOURCE 600
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *filename = "example.bin";
    int fd;

    fd = open(filename, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    off_t offset = 0;
    off_t length = 1024 * 1024; 

    int ret = posix_fallocate(fd, offset, length);
    if (ret != 0) {
        fprintf(stderr, "posix_fallocate: %s\n", strerror(ret));
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Allocated %ld bytes for %s\n", (long)length, filename);
    close(fd);
    return 0;
}
