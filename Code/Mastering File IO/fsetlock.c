#define _GNU_SOURCE   
#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>

FILE *fp;

void get_lock_type() {
    
    int mode = __fsetlocking(fp, FSETLOCKING_QUERY);
    printf("\nLocking mode: %s\n",
           mode == FSETLOCKING_INTERNAL ? "INTERNAL" :
           mode == FSETLOCKING_BYCALLER ? "BYCALLER" : "UNKNOWN");
}

int main() {

    fp = fopen("file.txt", "w");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    get_lock_type();
    printf("\nChanging locking..");
    __fsetlocking(fp, FSETLOCKING_BYCALLER);
    get_lock_type();

    for (int i = 0; i < 5; i++) {
        fprintf(fp, "Line %d\n", i);
    }

    fclose(fp);
    return 0;
}
