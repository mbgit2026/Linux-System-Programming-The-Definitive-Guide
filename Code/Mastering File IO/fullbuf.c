#include <stdio.h>
#include <unistd.h>

int main(void) {
    setvbuf(stdout, NULL, _IOFBF, 1024);

    printf("This is full buffering.\n");
    printf("Message 1 (not yet visible)...\n");
    printf("Message 2 (not yet visible)...\n");


    sleep(3);
    fflush(stdout);
    return 0;
}
