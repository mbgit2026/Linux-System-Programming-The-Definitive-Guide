#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *extension_filter_str = NULL;

int extension_filter(const struct dirent *entry) {
    const char *ext = strrchr(entry->d_name, '.');
    return ext && extension_filter_str && strcmp(ext, extension_filter_str) == 0;
}

int main(int argc, char *argv[])
{
    struct dirent **namelist;
    int n, i=0;
    const char *dir_path = ".";

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <extension> <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    extension_filter_str = argv[1]; 
    dir_path = argv[2];

    n = scandir(argv[2], &namelist, extension_filter, alphasort);

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
