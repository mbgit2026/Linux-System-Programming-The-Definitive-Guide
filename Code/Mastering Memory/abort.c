#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Calling abort()...\n");

    abort();

    printf("This will not be printed.\n");
    return 0;
}
