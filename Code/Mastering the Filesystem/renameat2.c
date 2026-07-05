#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "%s <oldname> <newname> <exchange|nover>\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    if (strcmp(argv[3], "exchange") == 0) {
        printf("Exchange file..");
        if (renameat2(AT_FDCWD, argv[1], AT_FDCWD, argv[2], RENAME_EXCHANGE) == -1) {
            perror("rename");
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[3], "nover") == 0) {
        printf("Rename not overwrite..");
        if (renameat2(AT_FDCWD, argv[1], AT_FDCWD, argv[2], RENAME_NOREPLACE) == -1) {
            perror("rename");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);

}