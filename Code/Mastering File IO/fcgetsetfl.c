#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int check_fd_flags(int fd) {

    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl(F_GETFL)");
        return 0;
    }

    switch (flags & O_ACCMODE) {
        case O_RDONLY: printf("  Access mode: O_RDONLY\n"); break;
        case O_WRONLY: printf("  Access mode: O_WRONLY\n"); break;
        case O_RDWR:   printf("  Access mode: O_RDWR\n");   break;
        default:       printf("  Access mode: UNKNOWN\n");  break;
    }

    printf("  O_APPEND   : %s\n",  (flags & O_APPEND)   ? "true" : "false");
    printf("  O_NONBLOCK : %s\n",  (flags & O_NONBLOCK)? "true" : "false");
    printf("  O_SYNC     : %s\n",  (flags & O_SYNC)    ? "true" : "false");
    printf("  O_DSYNC    : %s\n",  (flags & O_DSYNC)   ? "true" : "false");
    printf("  O_RSYNC    : %s\n",  (flags & O_RSYNC)   ? "true" : "false");
    printf("  O_DIRECT   : %s\n",  (flags & O_DIRECT)  ? "true" : "false");
    printf("  O_NOATIME  : %s\n",  (flags & O_NOATIME) ? "true" : "false");
    printf("  O_PATH     : %s\n",  (flags & O_PATH)    ? "true" : "false");

    return flags;
}

int main() {
    int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
  
    printf("\n--- Initial flags ---\n");
    int flags = check_fd_flags(fd);

    if (fcntl(fd, F_SETFL, flags | O_APPEND) == -1) {
        perror("fcntl(F_SETFL)");
        close(fd);
        exit(1);
    }

    printf("\n--- Final flags ---\n");
    flags = check_fd_flags(fd);
    close(fd);
    return 0;
}
