#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>

int main(void) {
    int n;
    printf("Enter buffer size: ");
    scanf("%d", &n);

    char *buf;

    if (n <= 1024) {
        printf("Using alloca() for %d bytes\n", n);
        buf = alloca(n);
    } else {
        printf("Using malloc() for %d bytes\n", n);
        buf = malloc(n);
        if (!buf) {
            printf("malloc failed\n");
            return 1;
        }
    }

    for (int i = 0; i < n; i++)
        buf[i] = 'A';

    printf("Buffer filled.\n");

    if (n > 1024)
        free(buf);

    return 0;
}
