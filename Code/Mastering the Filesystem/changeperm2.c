#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <mode_in_octal>\n", argv[0]);
        return 1;
    }

    char *filename = argv[1];
    mode_t new_mode = strtol(argv[2], NULL, 8);

    struct stat file_stat;
    if (stat(filename, &file_stat) == -1) {
        perror("stat");
        return 1;
    }

    printf("Current permissions: %o\n", file_stat.st_mode & 0777);

    if (chmod(filename, new_mode) == -1) {
        perror("chmod");
        return 1;
    }

    printf("Permissions changed to: %o\n", new_mode);
    return 0;
}

