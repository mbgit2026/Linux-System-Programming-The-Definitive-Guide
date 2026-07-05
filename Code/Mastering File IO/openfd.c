#include <stdio.h>
#include <unistd.h>

int main() {

    int n;
    char buffer[32];

    printf("\nInsert file descriptor: ");
    scanf("%d", &n);

    if (read(n, buffer, 32) == -1)
        perror("read");

    printf("\nBuffer: %s", buffer);
    return 0;

}