#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(void) {


  /*  int *marr = malloc(10 * sizeof(int));

  //  int *carr = calloc(10, sizeof(int));

    for (int i=0; i<10; i++) {
        printf("marr[%d] = %d\n", i, marr[i]);
  //      printf("carr[%d] = %d\n", i, carr[i]);
    }

    free(marr);
 //   free(carr);
*/


int n = 2;

// Use malloc (uninitialized memory)
int *marr = malloc(n * sizeof(int));
if (!marr) {
    perror("malloc failed");
    return 1;
}

printf("malloc array contents (may be garbage):\n");
for (int i = 0; i < n; i++) {
    printf("%d ", marr[i]);
}
printf("\n");

free(marr);

// Use calloc (zero-initialized memory)
int *carr = calloc(n, sizeof(int));
if (!carr) {
    perror("calloc failed");
    return 1;
}

printf("calloc array contents (guaranteed zeros):\n");
for (int i = 0; i < n; i++) {
    printf("%d ", carr[i]);
}
printf("\n");

free(carr);

return 0;

}
