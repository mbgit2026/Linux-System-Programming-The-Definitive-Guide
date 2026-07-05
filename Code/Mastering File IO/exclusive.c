#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(void) {
    const char *filename = "testfile.txt";

    printf("Trying to open/create file without O_EXCL...\n");
    int fd1 = open(filename, O_CREAT | O_WRONLY, 0644);
    if (fd1 == -1) {
        perror("open without O_EXCL failed");
        return 1;
    } else {
        printf("open without O_EXCL succeeded, fd=%d\n", fd1);
        close(fd1);
    }

    printf("\nTrying to open/create file with O_EXCL...\n");
    int fd2 = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd2 == -1) {
        if (errno == EEXIST) {
            printf("open with O_EXCL failed: file already exists!\n");
        } else {
            perror("open with O_EXCL failed for another reason");
        }
    } else {
        printf("open with O_EXCL succeeded, fd=%d\n", fd2);
        close(fd2);
    }

    return 0;
}

