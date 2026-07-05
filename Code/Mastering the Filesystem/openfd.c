#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *dir_path = argv[1];
    const char *file_name = argv[2];

    int dirfd = open(dir_path, O_RDONLY | O_DIRECTORY);
    if (dirfd < 0) {
        perror("open (directory)");
        return EXIT_FAILURE;
    }

    int filefd = openat(dirfd, file_name, O_RDONLY);
    if (filefd < 0) {
        perror("openat (file)");
        close(dirfd);
        return EXIT_FAILURE;
    }

    printf("Successfully opened '%s/%s' with file descriptor %d\n", dir_path, file_name, filefd);


    close(filefd);
    close(dirfd);
    return EXIT_SUCCESS;
}
