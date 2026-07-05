#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    size_t old_size = 4096;
    size_t new_size = 8192;

    // 1. Map an anonymous memory region
    void *addr = mmap(NULL, old_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("Original mapping at: %p (size=%zu)\n", addr, old_size);
    strcpy((char*)addr, "Hello from the original mapping!");

    // 2. Expand it using mremap
    void *new_addr = mremap(addr, old_size, new_size, MREMAP_MAYMOVE);
    if (new_addr == MAP_FAILED) {
        perror("mremap");
        munmap(addr, old_size);
        exit(1);
    }

    printf("New mapping at: %p (size=%zu)\n", new_addr, new_size);

    // Print old contents (they must still be there)
    printf("Data after remap: %s\n", (char*)new_addr);

    // Write into the new (extended) part
    strcpy((char*)new_addr + old_size, " Extended data!");

    printf("Full data: %s%s\n", (char*)new_addr, (char*)new_addr + old_size);

    // 3. Unmap at the end
    munmap(new_addr, new_size);

    return 0;
}
