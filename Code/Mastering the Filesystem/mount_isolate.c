#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <errno.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <mount_point>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *source_path = argv[1];   
    const char *target_path = "/mnt/newclone";   

    int mount_fd = open_tree(AT_FDCWD, source_path, OPEN_TREE_CLOEXEC); 

    if (mount_fd < 0) {
        perror("open_tree");
        exit(EXIT_FAILURE);
    }

    struct mount_attr attr = {
        .attr_set = MOUNT_ATTR_RDONLY,
        .attr_clr = 0,
        .propagation = 0,
        .userns_fd = 0
    };

    if (mount_setattr(mount_fd, "", AT_EMPTY_PATH, &attr, sizeof(attr)) < 0) {
        perror("mount_setattr");
        close(mount_fd);
        exit(EXIT_FAILURE);
    }

    if (move_mount(mount_fd, "", AT_FDCWD, target_path, MOVE_MOUNT_F_EMPTY_PATH) < 0) {
        perror("move_mount");
        close(mount_fd);
        exit(EXIT_FAILURE);
    }

    printf("Successfully cloned, modified, and moved mount to: %s\n", target_path);
    close(mount_fd);
    exit(EXIT_SUCCESS);
}
