#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
    char template1[] = "/tmp/tmpfile1-XXXXXX";
    char template2[] = "/tmp/tmpfile2-XXXXXX.txt";
    char template3[] = "/tmp/tmpfile3-XXXXXX.log";

    int fd1 = mkostemp(template1, O_CLOEXEC);
    if (fd1 == -1) {
        perror("mkostemp");
        exit(EXIT_FAILURE);
    }
    printf("mkostemp created: %s\n", template1);
    close(fd1);

    int fd2 = mkstemps(template2, 4);
    if (fd2 == -1) {
        perror("mkstemps");
        exit(EXIT_FAILURE);
    }
    printf("mkstemps created: %s\n", template2);
    close(fd2);

    int fd3 = mkostemps(template3, 4, O_CLOEXEC);
    if (fd3 == -1) {
        perror("mkostemps");
        exit(EXIT_FAILURE);
    }
    printf("mkostemps created: %s\n", template3);
    close(fd3);

    return 0;
}
