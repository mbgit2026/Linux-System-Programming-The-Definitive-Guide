/*
 * mmap_demo.c
 *
 * Demonstrates:
 *  1) mapping a file rounded to whole pages,
 *  2) bytes in the final partial page (beyond file end but inside the mapping)
 *     are zero-initialized,
 *  3) extending the file (append) and observing the mapped memory reflect the new bytes,
 *  4) then attempting an access one byte past the mapping to provoke a fault.
 *
 * Usage: ./mmap_demo existing_file
 *
 * WARNING: the final read intentionally accesses one byte past the mapping and
 * will normally crash the process (SIGBUS or SIGSEGV). This is intentional.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    long page = sysconf(_SC_PAGE_SIZE);
    if (page <= 0) page = 4096;
    printf("PAGE SIZE = %ld\n", page);

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return EXIT_FAILURE;
    }

    off_t file_size = st.st_size;
    printf("Initial file size = %jd\n", (intmax_t)file_size);
    if (file_size == 0) {
        printf("Note: file is empty — the demo still works but shows zeros.\n");
    }

    /* choose a mapping size that is rounded up to pages */
    size_t pages = (file_size + page - 1) / page;         /* 0 if file_size == 0 */
    if (pages == 0) pages = 1;                           /* map at least one page */
    size_t map_len = pages * (size_t)page;

    printf("Mapping %zu bytes (%zu pages)\n", map_len, pages);

    void *addr = mmap(NULL, map_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return EXIT_FAILURE;
    }

    char *p = (char*)addr;

    /* show a few bytes near the end of the file */
    printf("\n--- snapshot before file extension ---\n");
    printf("file bytes available: [0 .. %jd]\n", (intmax_t)file_size - 1);
    printf("mapped region: %p .. %p (length=%zu)\n", (void*)&p[0], (void*)&p[map_len-1], map_len);

    off_t check = file_size; /* first byte just past file end */
    if (check < (off_t)map_len) {
        printf("byte at offset %jd (first byte beyond file end but inside mapping): ", (intmax_t)check);
        unsigned char v = p[check];
        if (v == 0) printf("0x00 (zero)\n");
        else printf("0x%02x '%c'\n", v, (v >= 32 && v < 127) ? v : '?');
    } else {
        printf("No partial page: mapping does not extend past file end.\n");
    }

    /* Append new data to the file in a safe way */
    const char add[] = "HELLO";
    ssize_t wrote = pwrite(fd, add, sizeof(add)-1, file_size);
    if (wrote == -1) {
        perror("pwrite");
        munmap(addr, map_len);
        close(fd);
        return EXIT_FAILURE;
    }
    printf("\nAppended %zd bytes to file using pwrite()\n", wrote);

    /* flush file changes to pagecache/disk */
    if (fsync(fd) == -1) {
        perror("fsync");
        /* continue anyway */
    }

    /* msync to ensure mapping sees the updated backing store */
    if (msync(addr, map_len, MS_SYNC) == -1) {
        perror("msync");
        /* continue anyway */
    }

    /* refresh stat to see new file size */
    if (fstat(fd, &st) == -1) {
        perror("fstat(after)");
    }
    file_size = st.st_size;
    printf("New file size = %jd\n", (intmax_t)file_size);

    /* Now read the bytes that were previously zero (they may now contain the appended data,
       provided the appended bytes fall inside the previously mapped region) */
    printf("\n--- snapshot after file extension ---\n");
    off_t i_start = (file_size > (off_t)map_len) ? (off_t)map_len - 16 : (file_size > 16 ? file_size - 16 : 0);
    for (off_t i = i_start; i < (off_t)file_size && i < (off_t)map_len; ++i) {
        unsigned char v = p[i];
        printf("%jd: 0x%02x '%c'\n", (intmax_t)i, v, (v >= 32 && v < 127) ? v : '.');
    }

    printf("\nIf the appended bytes landed inside the originally-mapped region, you should see them above.\n");
    printf("If the append extended the file beyond the original mapping, those bytes are not reachable\n");
    printf("until you remap. Accessing addresses outside the mapping will cause SIGBUS/SIGSEGV.\n");

    /* Intentionally access one byte past the mapped region to provoke a fault.
       This will normally kill the program with SIGBUS or SIGSEGV. */
    printf("\nNow intentionally accessing one byte past the mapping (%zu) to provoke a fault...\n", map_len);
    fflush(stdout);

    volatile unsigned char crash = p[map_len]; /* <-- out-of-bounds read: intentional crash */
    /* The line above should trigger SIGBUS or SIGSEGV on a correct system.
       If it somehow returns, print the value (unlikely). */
    printf("Unexpectedly survived read: 0x%02x\n", crash);

    munmap(addr, map_len);
    close(fd);
    return EXIT_SUCCESS;
}
