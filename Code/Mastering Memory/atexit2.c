#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void good(void) {
    printf("Good\n");
}

void bye(void) {
    printf("bye\n");
}

int main(void) 
{
    int f1, f2;

    f1 = atexit(bye);
    if (f1 != 0) {
        fprintf(stderr, "cannot set bye function\n");
        exit(EXIT_FAILURE);
    }

    f2 = atexit(good);
    if (f2 != 0) {
        fprintf(stderr, "cannot set good function\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

    // return 0;
    // _exit(0);
    // _Exit(0);
}
