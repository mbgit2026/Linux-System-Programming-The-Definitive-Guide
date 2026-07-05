#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

int main (int argc, char *argv[]) {

    char *dirc, *basec, *bname, *dname;

    if (argc != 2) {
        printf("\nUsage: %s <path>", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *path = argv[1];

    dirc = strdup(path);
    basec = strdup(path);
    dname = dirname(dirc);
    bname = basename(basec);
    printf("DIRNAME=%s \nBASENAME=%s\n", dname, bname);


    exit(EXIT_SUCCESS);

}