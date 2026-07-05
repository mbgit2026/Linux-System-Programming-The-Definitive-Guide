#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    size_t num_pages = 4;
    size_t len = num_pages * page_size;

    /* Allocate anonymous memory */
    char *addr = mmap(NULL, len,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS,
                      -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Touch all pages so they become resident */
    for (size_t i = 0; i < num_pages; i++) {
        addr[i * page_size] = 1;
    }

    unsigned char vec[num_pages];

    /* Check residency before MADV_COLD */
    mincore(addr, len, vec);
    printf("Before MADV_COLD:\n");
    for (size_t i = 0; i < num_pages; i++) {
        printf("  page %zu: resident = %s\n",
               i, (vec[i] & 1) ? "yes" : "no");
    }

    /* Advise the kernel that pages are cold */
    if (madvise(addr, len, MADV_COLD) == -1) {
        perror("madvise");
        return 1;
    }

    /*
     * The kernel may reclaim these pages at any time.
     * They may still appear resident until memory pressure occurs.
     */

    mincore(addr, len, vec);
    printf("\nAfter MADV_COLD:\n");
    for (size_t i = 0; i < num_pages; i++) {
        printf("  page %zu: resident = %s\n",
               i, (vec[i] & 1) ? "yes" : "no");
    }

    munmap(addr, len);
    return 0;
}
