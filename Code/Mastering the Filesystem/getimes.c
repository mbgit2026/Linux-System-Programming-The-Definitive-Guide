#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    struct stat sb;
    int fd;
    char newstring[11] = "New string\0";

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (lstat(argv[1], &sb) == -1) {
 		perror("lstat");
 		exit(EXIT_FAILURE);
 	}	
    
 	printf("\nAccess: %s", ctime(&sb.st_atime));
 	printf("\nModify: %s", ctime(&sb.st_mtime));
 	printf("\nChange: %s", ctime(&sb.st_ctime));
    fd = open(argv[1], O_WRONLY | O_CREAT);
    write(fd, newstring, 10);
    close(fd);

    if (lstat(argv[1], &sb) == -1) {
 		perror("lstat");
 		exit(EXIT_FAILURE);
 	}
 	printf("\nAccess: %s", ctime(&sb.st_atime));
 	printf("\nModify: %s", ctime(&sb.st_mtime));
 	printf("\nChange: %s", ctime(&sb.st_ctime));
    
    close(fd);
    exit(EXIT_SUCCESS);
}