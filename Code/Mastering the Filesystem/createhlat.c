
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *oldpath;
    char *newpath;

    if (argc != 3) {
        fprintf(stderr, "%s <oldpath> <newpath> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    oldpath = argv[1];
    newpath = argv[2]; 

    if (linkat(AT_FDCWD, oldpath, AT_FDCWD, newpath, AT_SYMLINK_FOLLOW) == -1) {
        perror("link");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}