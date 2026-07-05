#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    size_t num_pages = 4;
    size_t length = num_pages * page_size;

    char *addr = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    unsigned char vec[num_pages];
    if (mincore(addr, length, vec) == -1) {
        perror("mincore");
        return 1;
    }

    printf("Before access:\n");
    for (size_t i = 0; i < num_pages; i++) 
        printf("  page %zu: resident = %s\n", i, (vec[i] & 1) ? "yes" : "no");
    

    addr[1 * page_size] = 1;
    addr[3 * page_size] = 1;
    if (mincore(addr, length, vec) == -1) {
        perror("mincore");
        return 1;
    }

    printf("\nAfter access:\n");
    for (size_t i = 0; i < num_pages; i++) 
        printf("  page %zu: resident = %s\n",i, (vec[i] & 1) ? "yes" : "no");
    
    munmap(addr, length);
    return 0;
}
