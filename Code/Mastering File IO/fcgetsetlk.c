#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main() {
    
    const char *filename = "file.txt";
    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct flock fl;

    // Set up a write lock on first 50 bytes (blocking)
    fl.l_type = F_WRLCK;      // exclusive lock
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 50;
    fl.l_pid = 0;

    printf("Trying to acquire write lock on first 50 bytes...\n");
    if (fcntl(fd, F_SETLKW, &fl) == -1) {  // blocks if another process holds a conflicting lock
        perror("fcntl - F_SETLKW");
        close(fd);
        exit(EXIT_FAILURE);
    }
    printf("Write lock acquired.\n");

    // Check if another process holds a lock on the same region
    struct flock test;
    test.l_type = F_WRLCK;  // trying to acquire write lock
    test.l_whence = SEEK_SET;
    test.l_start = 0;
    test.l_len = 50;
    test.l_pid = 0;

    if (fcntl(fd, F_GETLK, &test) == -1) {
        perror("fcntl - F_GETLK");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (test.l_type == F_UNLCK) {
        printf("No conflicting locks found.\n");
    } else {
        printf("Lock conflict! PID %d holds a %s lock.\n", 
            test.l_pid, test.l_type == F_RDLCK ? "read" : "write");
    }

    printf("Press Enter to release lock...\n");
    getchar();

    // Unlock
    fl.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLKW, &fl) == -1) {
        perror("fcntl - unlock");
    } else {
        printf("Lock released.\n");
    }

    close(fd);
    return 0;
}
