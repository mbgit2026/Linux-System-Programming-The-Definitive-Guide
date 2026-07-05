#include <linux/fs.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {

    int fd;
    int flags;
    
    if (argc != 2) {
        printf("\nUsage %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (ioctl(fd, FS_IOC_GETFLAGS, &flags) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    flags |= FS_NOATIME_FL;
    if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    close(fd);

    if (flags & FS_IMMUTABLE_FL)    printf("Immutable flag is set\n");
    if (flags & FS_APPEND_FL)       printf("Append-only flag is set\n");
    if (flags & FS_NOATIME_FL)      printf("Noatime flag is set\n");
    if (flags & FS_DIRSYNC_FL)      printf("Dirsync flag is set\n");
    if (flags & FS_SYNC_FL)         printf("Sync flag is set\n");

    exit(EXIT_SUCCESS);
}
