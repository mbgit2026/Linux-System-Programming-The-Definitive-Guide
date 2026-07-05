#include <stdio.h>
#include <stdlib.h>

void exitfunc(int status, void *arg) {
    printf("Exit status: %d\n", status);
    printf("%s\n", (char *)arg);
}

int main(void) {

    if (on_exit(exitfunc, "Goodbye") != 0) {
        fprintf(stderr, "Can not set exitfunc\n");
        return 1;
    }

    printf("In the main\n");
    printf("Exiting...\n\n");
    exit(42);
}
