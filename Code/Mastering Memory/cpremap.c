#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

#define MAXPATH 256

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s dest\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd_dst = -1, fd_src = -1;
    struct stat st_src;
    char srcname[MAXPATH];

    void *dst = NULL;
    size_t dst_size = 1;

    fd_dst = open(argv[1], O_RDWR | O_CREAT, 0644);
    if (fd_dst == -1) {
        perror("open dest");
        return EXIT_FAILURE;
    }

    /* Initial size = 1 byte */
    if (ftruncate(fd_dst, 1) == -1) {
        perror("ftruncate");
        close(fd_dst);
        return EXIT_FAILURE;
    }

    dst = mmap(NULL, dst_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_dst, 0);
    if (dst == MAP_FAILED) {
        perror("mmap dest");
        close(fd_dst);
        return EXIT_FAILURE;
    }

    while (1) {
        printf("\nEnter source file (or 'quit'): ");
        fflush(stdout);

        if (!fgets(srcname, sizeof(srcname), stdin))
            break;

        srcname[strcspn(srcname, "\n")] = '\0';

        if (strcmp(srcname, "quit") == 0)
            break;

        fd_src = open(srcname, O_RDONLY);
        if (fd_src == -1) {
            perror("open src");
            continue;
        }

        if (fstat(fd_src, &st_src) == -1) {
            perror("fstat src");
            close(fd_src);
            continue;
        }

        if (st_src.st_size == 0) {
            printf("Source file is empty, skipping\n");
            close(fd_src);
            continue;
        }

        /* Resize destination file */
        if (ftruncate(fd_dst, st_src.st_size) == -1) {
            perror("ftruncate dest");
            close(fd_src);
            continue;
        }

        /* Remap destination */
        void *new_dst = mremap(dst, dst_size, st_src.st_size, MREMAP_MAYMOVE);
        if (new_dst == MAP_FAILED) {
            perror("mremap");
            close(fd_src);
            continue;
        }

        dst = new_dst;
        dst_size = st_src.st_size;

        /* Map source */
        void *src = mmap(NULL, st_src.st_size, PROT_READ, MAP_PRIVATE, fd_src, 0);
        if (src == MAP_FAILED) {
            perror("mmap src");
            close(fd_src);
            continue;
        }

        memcpy(dst, src, st_src.st_size);
        munmap(src, st_src.st_size);
        close(fd_src);
        printf("Copied %ld bytes\n", (long)st_src.st_size);
    }

    munmap(dst, dst_size);
    close(fd_dst);
    return 0;
}

