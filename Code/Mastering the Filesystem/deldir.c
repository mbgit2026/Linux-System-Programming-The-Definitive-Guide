#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <dirname>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((rmdir(argv[1])) == -1) {
        perror("rmdir");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}