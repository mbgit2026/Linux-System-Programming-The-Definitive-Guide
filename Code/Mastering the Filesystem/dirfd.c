#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    DIR *dir;
    int fd;
    struct stat sb;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    fd = dirfd(dir);
    if (fd == -1) {
        perror("dirfd");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    printf("Directory: %s\n", argv[1]);
    printf("Inode: %lu\n", (unsigned long) sb.st_ino);
    printf("Mode: %o\n", sb.st_mode);
    printf("Size: %ld\n", (long) sb.st_size);

    closedir(dir);
    return 0;
}