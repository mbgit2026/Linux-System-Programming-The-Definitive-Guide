#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <path> <major> <minor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *path = argv[1];
    int major = atoi(argv[2]);
    int minor = atoi(argv[3]);

    dev_t dev = makedev(major, minor);

    if (mknod(path, S_IFBLK | 0660, dev) == -1) {
        perror("mknod");
        exit(EXIT_FAILURE);
    }

    printf("Block special file '%s' created with major %d and minor %d.\n", path, major, minor);
    return EXIT_SUCCESS;
}
