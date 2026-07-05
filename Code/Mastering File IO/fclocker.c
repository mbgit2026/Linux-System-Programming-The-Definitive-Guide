#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(void) {
    int fd;
    struct flock fl;

    fd = open("file.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    memset(&fl, 0, sizeof(fl));
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 100;

    printf("[locker] Trying to acquire OFD write lock...\n");

    if (fcntl(fd, F_OFD_SETLKW, &fl) == -1) {
        perror("fcntl - F_OFD_SETLKW");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("[locker] Lock acquired! Holding it...\n");
    printf("[locker] Press Enter to release the lock.\n");
    getchar();

    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_OFD_SETLKW, &fl) == -1) {
        perror("fcntl - unlock");
    } else {
        printf("[locker] Lock released.\n");
    }

    close(fd);
    return 0;
}
