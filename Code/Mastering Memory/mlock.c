#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize == -1) {
        perror("sysconf");
        return 1;
    }

    /* Allocate one page */
    void *buf = aligned_alloc(pagesize, pagesize);
    if (!buf) {
        perror("aligned_alloc");
        return 1;
    }

    /* Touch the page so it is faulted in */
    memset(buf, 0xAA, pagesize);

    /* Lock the page in RAM */
    if (mlock(buf, pagesize) == -1) {
        perror("mlock");
        free(buf);
        return 1;
    }

    printf("Memory locked at address %p (%ld bytes)\n", buf, pagesize);
    printf("Press Enter to unlock and exit...\n");
    getchar();

    /* Unlock and free */
    if (munlock(buf, pagesize) == -1)
        perror("munlock");

    free(buf);
    return 0;
}
