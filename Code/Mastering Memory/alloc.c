#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {

   void *ptr = malloc(6 * sizeof(int));
   //int *ptr = malloc(10 * 1024 * 1024); // 10 MB

    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    free(ptr);
    return 0;
}


