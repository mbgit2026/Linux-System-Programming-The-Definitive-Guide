#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>


int main () {

    char *newpath, *current;
    char buf[1024];
    struct dirent *dr;
    DIR *dirp;

    while (1==1) {
        printf("\n\nEnter the new folder: ");
        scanf("%s", newpath);

        chdir(newpath);
        current = getcwd(buf, 1024);
        printf("New path: %s\n", current);

        if ((dirp = opendir(current)) == NULL) {
            perror("opendir");
            exit(EXIT_FAILURE);
        }
    
        while ((dr = readdir(dirp)) != NULL) {    
            printf("\n%s", dr->d_name);
        }

    }

    exit(EXIT_SUCCESS);
}