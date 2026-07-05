#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int main (int argc, char *argv[]) {

    DIR *dirp;
    struct dirent *dr;
    int i;
    long cur;

    if (argc != 3) {
        printf("\nUsage: %s <dirpath> <entry>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((dirp = opendir(argv[1])) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    for (i=0; i <= atoi(argv[2]); i++) {
        dr =readdir(dirp);
        cur = telldir(dirp);
    }

    seekdir(dirp, cur);
    dr = readdir(dirp);
    printf("\nENTRY %d: %s\n",atoi(argv[2]), dr->d_name);

    rewinddir(dirp);

    printf("\n----- List of files: ----\n");
    while ((dr = readdir(dirp)) != NULL) {    
        printf("\n%s", dr->d_name);
    }
    

    closedir(dirp);
    exit(EXIT_SUCCESS);
}