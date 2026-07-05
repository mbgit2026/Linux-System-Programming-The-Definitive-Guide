#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int *addr;
    pid_t pid;

    addr=mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE, MAP_SHARED| MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror ("mmap");
        return EXIT_FAILURE;
    }

    *addr = 5;

    pid = fork();

    if (pid == -1) {
        perror ("fork");
        return 1;
    } else if (pid == 0) {
        printf ("\nChild  - pid: %d value = %d", getpid(), *addr);
        *addr = 8;
    } else {
        wait(NULL);
        printf ("\nParent - pid: %d value = %d", getpid(), *addr);
        if ((munmap(addr, sizeof(int))) == -1)
            perror ("munmap");
    }

    return EXIT_SUCCESS;
}