#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>


int main(void)
{
    size_t len = 4096;

    /* Create secret memory */
    int fd = syscall(SYS_memfd_secret, 0);
    if (fd == -1) {
        perror("memfd_secret");
        return EXIT_FAILURE;
    }

    /* Set size */
    if (ftruncate(fd, len) == -1) {
        perror("ftruncate");
        close(fd);
        return EXIT_FAILURE;
    }

    /* Map secret memory */
    char *addr = mmap(NULL, len,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    /* Use the secret memory */
    strcpy(addr, "Top secret data");
    printf("Read from secret memory: %s\n", addr);

    /* Cleanup */
    munmap(addr, len);
    close(fd);

    return EXIT_SUCCESS;
}
