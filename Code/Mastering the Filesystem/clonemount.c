#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/mount.h>
#include <linux/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

static inline int mount_setattr(int dirfd, const char *pathname, unsigned int flags,
                                struct mount_attr *attr, size_t size) {
    return syscall(SYS_mount_setattr, dirfd, pathname, flags, attr, size);
}

static inline int open_tree(int dirfd, const char *filename, unsigned int flags) {
    return syscall(SYS_open_tree, dirfd, filename, flags);
}

static inline int move_mount(int from_dirfd, const char *from_pathname, int to_dirfd,
                            const char *to_pathname, unsigned int flags) {
    return syscall(SYS_move_mount, from_dirfd, from_pathname, to_dirfd, to_pathname, flags);
}

static const struct option longopts[] = {
    {"map-mount",       required_argument,  NULL,  'a'},
    {"recursive",       no_argument,        NULL,  'b'},
    {"read-only",       no_argument,        NULL,  'c'},
    {"block-setid",     no_argument,        NULL,  'd'},
    {"block-devices",   no_argument,        NULL,  'e'},
    {"block-exec",      no_argument,        NULL,  'f'},
    {"no-access-time",  no_argument,        NULL,  'g'},
    { NULL,             0,                  NULL,   0 },
};

int main(int argc, char *argv[])
{
    int                fd_userns = -1;
    int                fd_tree;
    int                index = 0;
    int                ret;
    bool               recursive = false;
    const char         *source;
    const char         *target;
    struct mount_attr  *attr = &(struct mount_attr){};

    while ((ret = getopt_long_only(argc, argv, "", longopts, &index)) != -1) {
        switch (ret) {
        case 'a':
            fd_userns = open(optarg, O_RDONLY | O_CLOEXEC);
            if (fd_userns == -1)
                err(EXIT_FAILURE, "open(%s)", optarg);
            break;
        case 'b':
            recursive = true;
            break;
        case 'c':
            attr->attr_set |= MOUNT_ATTR_RDONLY;
            break;
        case 'd':
            attr->attr_set |= MOUNT_ATTR_NOSUID;
            break;
        case 'e':
            attr->attr_set |= MOUNT_ATTR_NODEV;
            break;
        case 'f':
            attr->attr_set |= MOUNT_ATTR_NOEXEC;
            break;
        case 'g':
            attr->attr_set |= MOUNT_ATTR_NOATIME;
            attr->attr_clr |= MOUNT_ATTR__ATIME;
            break;
        default:
            errx(EXIT_FAILURE, "Invalid argument specified");
        }
    }

    if ((argc - optind) < 2)
        errx(EXIT_FAILURE, "Missing source or target mount point");

    source = argv[optind];
    target = argv[optind + 1];

    fd_tree = open_tree(-1, source, OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC | AT_EMPTY_PATH | (recursive ? AT_RECURSIVE : 0));
    if (fd_tree == -1)
        err(EXIT_FAILURE, "open(%s)", source);

    if (fd_userns >= 0) {
        attr->attr_set  |= MOUNT_ATTR_IDMAP;
        attr->userns_fd = fd_userns;
    }

    ret = mount_setattr(fd_tree, "", AT_EMPTY_PATH | (recursive ? AT_RECURSIVE : 0), attr, sizeof(struct mount_attr));
    if (ret == -1)
        err(EXIT_FAILURE, "mount_setattr");

    close(fd_userns);

    ret = move_mount(fd_tree, "", -1, target, MOVE_MOUNT_F_EMPTY_PATH);
    if (ret == -1)
        err(EXIT_FAILURE, "move_mount() to %s", target);

    close(fd_tree);

    exit(EXIT_SUCCESS);
}
