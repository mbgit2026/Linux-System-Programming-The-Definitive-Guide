#include <stdio.h>
#include <unistd.h>

int main() 
{
    printf("Process ID: %d", (long)getpid());
    printf("\nParent process ID: %d", (long)getppid());

    return 0;
}