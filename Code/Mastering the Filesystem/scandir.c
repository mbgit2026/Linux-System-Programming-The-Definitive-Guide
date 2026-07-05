#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct dirent **namelist;
    int n, i=0;

    if (argc != 2) {
        printf("\nUsage: %s <directory>", argv[0]);
        exit(EXIT_FAILURE);
    }

    n = scandir(argv[1], &namelist, NULL, alphasort);

    if (n == -1) {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    while (i < n) {
        printf("%s\n", namelist[i]->d_name);
        free(namelist[i]);
        i++;
    }
    
    free(namelist);
    exit(EXIT_SUCCESS);
}
