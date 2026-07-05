#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <file> <block|noblock> <bytes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    int length = atoi(argv[3]);  // number of bytes to lock

    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Trying to acquire exclusive lock on %s (%d bytes)...\n", filename, length);

    int cmd;
    if (strcmp(argv[2], "block") == 0) {
        cmd = F_LOCK;   // Blocking
    } else if (strcmp(argv[2], "noblock") == 0) {
        cmd = F_TLOCK;  // Non-blocking
    } else {
        fprintf(stderr, "Invalid mode: %s (use 'block' or 'noblock')\n", argv[2]);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Lock the first `length` bytes
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (lockf(fd, cmd, length) == -1) {
        perror("lockf");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Exclusive lock acquired on first %d bytes! (PID: %d)\n", length, getpid());
    printf("Press Enter to release the lock and exit...\n");
    getchar();

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (lockf(fd, F_ULOCK, length) == -1) {
        perror("unlock");
    }

    close(fd);
    return 0;
}



