
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    FILE *fp;

    fp = freopen("output.txt", "w", stdout);
    if (fp == NULL) {
        perror("freopen");
        exit(EXIT_FAILURE);
    }

    printf("Hello, world!\n");
    printf("This text is written to the file instead of the terminal.\n");

    fclose(fp);
    exit(EXIT_SUCCESS);
}
