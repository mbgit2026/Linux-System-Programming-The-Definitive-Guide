#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <linux/mount.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage %s <mount_point>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *target = argv[1];

    int dirfd = open(target, O_DIRECTORY | O_RDONLY | O_CLOEXEC);
    if (dirfd < 0) {
        perror("open target");
        exit(EXIT_FAILURE);
    }

    struct mount_attr attr = {
        .attr_set = MOUNT_ATTR_RDONLY,
        .attr_clr = 0,
        .propagation = 0,
        .userns_fd = 0
    };

    int ret = syscall(SYS_mount_setattr, dirfd, "", AT_EMPTY_PATH, &attr, sizeof(attr));
    if (ret < 0) {
        perror("mount_setattr");
        close(dirfd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully set mount as read-only for: %s\n", target);
    close(dirfd);
    exit(EXIT_SUCCESS);
}
