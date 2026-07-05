#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#define MOUNT_DIR "/mnt/temp_mount"
#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <device> <file path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *device = argv[1];
    const char *file_path = argv[2];

    if (mkdir(MOUNT_DIR, 0755) < 0 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }

    if (mount(device, MOUNT_DIR, "iso9660", MS_RDONLY, NULL) < 0) {
        perror("mount");
        umount(MOUNT_DIR);
        exit(EXIT_FAILURE);
    }

    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", MOUNT_DIR, file_path);

    int fd = open(fullpath, O_RDONLY);
    if (fd < 0) {
        perror("read");
        umount(MOUNT_DIR);
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(fd, buf, BUF_SIZE)) > 0) {
        fwrite(buf, 1, n, stdout);
    }
    if (n < 0) {
        perror("write");
        umount(MOUNT_DIR);
        exit(EXIT_FAILURE);
    }

    close(fd);

    if (umount(MOUNT_DIR) < 0) {
        perror("umount");
    }

    exit(EXIT_SUCCESS);
}
