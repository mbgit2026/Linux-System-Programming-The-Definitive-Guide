#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/openat2.h>
#include <sys/syscall.h>
#include <string.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dir_path = argv[1];
    const char *file_name = argv[2];

    int dirfd = open(dir_path, O_RDONLY | O_DIRECTORY);
    if (dirfd == -1) {
        perror("open (dir)");
        return EXIT_FAILURE;
    }

    struct open_how how = {
        .flags = O_RDONLY,
        .mode = 0,
        .resolve = RESOLVE_BENEATH | RESOLVE_NO_SYMLINKS
    };

    int fd = syscall(SYS_openat2, dirfd, file_name, &how, sizeof(how));
    if (fd == -1) {
        perror("openat2");
        close(dirfd);
        return EXIT_FAILURE;
    }

    printf("Successfully opened '%s/%s' with openat2, fd=%d\n", dir_path, file_name, fd);

    close(fd);
    close(dirfd);
    return EXIT_SUCCESS;
}
