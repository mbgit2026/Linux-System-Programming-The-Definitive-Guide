#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include <bits/stat.h>

int main (int argc, char *argv[]) {

    struct stat sb;
    int fd;
    struct timespec newts[2];
    

    if (argc !=4) {
        printf("Usage: %s <filename> sec nsec");
        exit(EXIT_FAILURE);
    }

    
    newts[0].tv_sec = strtol (argv[2], NULL, 10);
    //newts[0].tv_nsec = strtol(argv[3], NULL, 10);
    newts[0].tv_nsec = UTIME_NOW;
    newts[1].tv_sec = strtol(argv[2], NULL, 10);
    //newts[1].tv_nsec = strtol(argv[3], NULL, 10);
    newts[1].tv_nsec = UTIME_NOW;
    

    if (lstat(argv[1], &sb) != 0)  {
        perror("lstat");
        exit(EXIT_FAILURE);
    }


    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (futimens(fd, newts) != 0) {
        perror("futimens");
        exit(EXIT_FAILURE);
    }
   
    exit(EXIT_SUCCESS);
}//Comment
