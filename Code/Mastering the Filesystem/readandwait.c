#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>


int main (int argc, char *argv[]) {

    int fd, fdnew;
    char buffer[256];
    char readfile;

    if (argc != 2) {
        fprintf(stderr, "%s <filename> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    printf("Proceed to read the content of the file? [y/n]");
    scanf("%c", &readfile);
    if (readfile == 'y') {
        read(fd, buffer, 256);
        printf("Content of the file: \n %s", buffer);
        fdnew = creat("newfile.txt", S_IRWXU);
        if (write(fdnew, buffer, strlen(buffer)) != -1) {
            printf("File written: newfile.txt");
        } else {
            perror("write");
        }
    }

    close(fd);
    close(fdnew);
    exit(EXIT_SUCCESS);

}