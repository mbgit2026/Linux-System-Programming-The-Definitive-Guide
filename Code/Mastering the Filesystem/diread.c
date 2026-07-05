#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main (int argc, char *argv[]) {

    DIR *dirp;
    struct dirent *dr;

    if (argc != 2) {
        printf("\nUsage: %s <dirpath>", argv[0]);
        exit(EXIT_FAILURE);
    }


    if ((dirp = opendir(argv[1])) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((dr = readdir(dirp)) != NULL) {    
        printf("\n%s", dr->d_name);
    }

    closedir(dirp);

    exit(EXIT_SUCCESS);
}