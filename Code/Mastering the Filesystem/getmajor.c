#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

int main (int argc, char *argv[]) {

    struct stat st;
    
    if (argc != 2) {
        printf("\nUsage: %s <device>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((stat(argv[1], &st) != 0)) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("\n%s : %d/%d",argv[1], major(st.st_rdev), minor(st.st_rdev));

    exit(EXIT_SUCCESS);
}
