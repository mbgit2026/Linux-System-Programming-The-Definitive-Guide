#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
    long pagesize = sysconf(_SC_PAGESIZE);
    printf("System page size: %ld bytes\n", pagesize);

    // Allocate one page, aligned to page size
    void *buffer = NULL;
    if (posix_memalign(&buffer, pagesize, pagesize) != 0) {
        perror("posix_memalign");
        return 1;
    }

    // Fill buffer with some text
    strcpy((char*)buffer, "Hello from a page-aligned buffer!\n");

    // Create a file to mmap
    int fd = open("mapped_output.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        free(buffer);
        return 1;
    }

    // Extend file to one page
    if (ftruncate(fd, pagesize) != 0) {
        perror("ftruncate");
        close(fd);
        free(buffer);
        return 1;
    }

    // Map the file into memory
    void *map = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        close(fd);
        free(buffer);
        return 1;
    }

    // Copy aligned data into the mapped area
    memcpy(map, buffer, pagesize);
    printf("Wrote aligned data into the mmap'd file.\n");

    // Sync changes to disk
    msync(map, pagesize, MS_SYNC);

    // Cleanup
    munmap(map, pagesize);
    close(fd);
    free(buffer);

    return 0;
}
