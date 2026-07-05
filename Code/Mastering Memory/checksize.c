#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return 1;
    }

    size_t size = st.st_size;
    char *addr = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    write(STDOUT_FILENO, addr, size);

    for (;;) {
        sleep(5);

        if (fstat(fd, &st) == -1)
            continue;

        if (st.st_size > size) {
            char *newaddr = mremap(addr, size, st.st_size, MREMAP_MAYMOVE);
            if (newaddr == MAP_FAILED) {
                perror("mremap");
                break;
            }
            addr = newaddr;
            size = st.st_size;

            write(STDOUT_FILENO, addr, size);
        }
    }
}
