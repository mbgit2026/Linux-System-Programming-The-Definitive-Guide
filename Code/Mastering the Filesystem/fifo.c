#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main (int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <pathname>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((mkfifo(argv[1], 0660)) != 0) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("\nFIFO %s created", argv[1]);
    exit(EXIT_SUCCESS);
}