#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
    size_t pagesize = sysconf(_SC_PAGE_SIZE);

    /* Allocate anonymous memory */
    char *secret = mmap(NULL, pagesize,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS,
                        -1, 0);
    if (secret == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    /* Store a secret key */
    strcpy(secret, "SUPER_SECRET_KEY_123456");

    printf("Secret stored at %p\n", secret);
    printf("PID: %d\n", getpid());

    /* Ensure page is faulted in */
    volatile char c = secret[0];
    (void)c;

    /* Ask kernel to page it out */
    if (madvise(secret, pagesize, MADV_PAGEOUT) == -1) {
        perror("madvise");
    }

    printf("Page-out requested. Sleeping...\n");
    sleep(600);

    munmap(secret, pagesize);
    return 0;
}
