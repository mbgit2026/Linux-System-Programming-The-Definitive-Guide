#include <linux/fs.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char *argv[]) {

    int fd;
    int flags;
    
    if (argc != 3) {
        printf("\nUsage %s <filename> <0/1>", argv[0]);
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

    if (atoi(argv[2]) == 1) {
        flags |= FS_IMMUTABLE_FL;
    } else if(atoi(argv[2]) == 0) {
        flags &= ~FS_IMMUTABLE_FL;
    } else {
        printf("\nSecond parameter must be <0/1>");
        exit(EXIT_FAILURE);
    }
    if (ioctl(fd, FS_IOC_SETFLAGS, &flags) == -1) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    close(fd);
    exit(EXIT_SUCCESS);
}