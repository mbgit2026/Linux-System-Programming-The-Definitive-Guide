#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    char buffer[64];

    printf("\nInsert data: ");
    if (scanf("%s", &buffer) == -1) {
        perror("scanf");
        if (fcntl(STDIN_FILENO, F_GETFD) == -1 ) 
            printf("\nInput is not allowed");
    }

    printf("\n%s", buffer);
    return 0;
}
