#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int
main(int argc, char *argv[])
{
    char *oldpath;
    char *newpath;

    if (argc != 3) {
        fprintf(stderr, "%s <oldpath> <newpath> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    oldpath = argv[1];
    newpath = argv[2]; 


    if (link(oldpath, newpath) == -1) {
        perror("link");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}