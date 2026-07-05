#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define NUM_BYTES 16  

int main() {
    int fd;
    unsigned char buffer[NUM_BYTES];

    fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("Failed to open /dev/urandom");
        return EXIT_FAILURE;
    }

    ssize_t bytesRead = read(fd, buffer, NUM_BYTES);
    if (bytesRead != NUM_BYTES) {
        perror("Failed to read enough random data");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("Random bytes from /dev/urandom:\n");
    for (size_t i = 0; i < NUM_BYTES; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
    close(fd);
    return EXIT_SUCCESS;
}
