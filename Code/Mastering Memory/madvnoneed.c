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

    char *addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Touch each page so it becomes resident */
    for (size_t i = 0; i < num_pages; i++) 
        addr[i * page_size] = (char)(i + 1);
    

    printf("Pages populated and written.\n");
    for (size_t i = 0; i < num_pages; i++) {
        char val = addr[i * page_size];
        printf("Before DONTNEED, page %zu value = %d\n", i, val);
    }


    /* Tell the kernel the pages are no longer needed */
    if (madvise(addr, len, MADV_DONTNEED) == -1) {
        perror("madvise");
        return 1;
    }

    printf("\nMADV_DONTNEED issued.\n");

    /*
     * For anonymous mappings:
     *  - the physical pages are discarded immediately
     *  - future reads return zero-filled pages
     *  - future writes fault in fresh pages
     */

    /* Read from the pages again */
    for (size_t i = 0; i < num_pages; i++) {
        char val = addr[i * page_size];
        printf("After DONTNEED, page %zu value = %d\n", i, val);
    }

    munmap(addr, len);
    return 0;
}
