#include <stdio.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: %s <file>", argv[0]);

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        warn("failed to open file: %s", argv[1]);
        warnx("Insert a file that exists");
    }

    printf("Program reached the end\n");
    return 0;
}
