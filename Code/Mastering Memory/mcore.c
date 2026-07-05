#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);

    int fd = open("newfile.txt", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    void *addr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

       if (madvise(addr, page_size, MADV_POPULATE_READ) == -1) {
            perror("madvise");
            return 1;
        }

    unsigned char vec;
    if (mincore(addr, page_size, &vec) == -1) {
        perror("mincore");
        return 1;
    }

    printf("Before access: resident = %s\n", (vec & 1) ? "yes" : "no");

    /* Touch the page to force it into memory */
    *(char *)addr = 42;

    if (mincore(addr, page_size, &vec) == -1) {
        perror("mincore");
        return 1;
    }

    printf("After access:  resident = %s\n", (vec & 1) ? "yes" : "no");

    munmap(addr, page_size);
    return 0;
}
