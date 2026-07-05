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

    if (fd = open(argv[1], O_RDWR) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    printf("\nFile open");

    if (unlink(argv[1]) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }
    printf("\nFile unlinked");

    printf("\nDo you want to save the file? [y/n]");
    scanf("%c", &readfile);
    if (readfile == 'y') {
        read(fd, buffer, 256);
        fdnew = creat("newfile.txt", S_IRWXU);
        if (write(fdnew, buffer, strlen(buffer)) != -1) 
            printf("File written: newfile.txt");
        else {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    close(fd);
    close(fdnew);
    exit(EXIT_SUCCESS);

}