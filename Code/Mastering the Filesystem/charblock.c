#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    struct stat sb;

    if (argc != 2) {
        printf("Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (stat(argv[1], &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    if (S_ISBLK(sb.st_mode)) {
        printf("%s is a block device\n", argv[1]);
    } else if (S_ISCHR(sb.st_mode)) {
        printf("%s is a character device\n", argv[1]);
    } else {
        printf("%s is neither a block nor a character device\n", argv[1]);
    }

    return EXIT_SUCCESS;
}