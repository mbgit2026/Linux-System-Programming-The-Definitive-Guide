#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    const char *filename = "testfile.bin";
    int fd = open(filename, O_CREAT | O_RDWR, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    off_t request_size = 20 * 1024 * 1024; 
    printf("Trying to allocate %ld bytes...\n", (long)request_size);

    if (fallocate(fd, 0, 0, request_size) == -1) {
        perror("fallocate allocate");
    } else {
        printf("Successfully allocated %ld bytes\n", (long)request_size);
        close(fd);
        exit(EXIT_SUCCESS);
    }

    // Now deallocate (punch hole) in the range [0, 5 MB)
    off_t hole_size = 5 * 1024 * 1024;
    if (fallocate(fd, FALLOC_FL_PUNCH_HOLE | FALLOC_FL_KEEP_SIZE, 0, hole_size) == -1) {
        perror("fallocate deallocate");
    } else {
        printf("Successfully deallocated %ld bytes from start of file\n", (long)hole_size);
    }

    close(fd);
    return 0;
}
