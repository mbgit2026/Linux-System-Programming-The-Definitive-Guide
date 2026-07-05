#include <stdio.h>
#include <unistd.h>

int main() {
    printf("\nExiting...");
     fflush(stdout);
    _exit(0);
}