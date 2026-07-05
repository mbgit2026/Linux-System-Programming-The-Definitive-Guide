#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {


    if (argc != 3) {
        printf("Usage: %s <target> <newlink>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (symlink(argv[1], argv[2]) != 0) {
        perror("symlink");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

}