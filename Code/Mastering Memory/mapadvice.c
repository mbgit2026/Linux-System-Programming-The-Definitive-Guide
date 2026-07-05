#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int fd;
    struct stat st;

    fd = open("newfile.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &st) == -1) {
        perror("fstat");
        return 1;
    }

    char *addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    if (argc == 1) {
        if (madvise(addr, st.st_size, MADV_DONTFORK) == -1) {
            perror("madvise");
            return 1;
        }
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("\nChild output:\n");
        write(STDOUT_FILENO, addr, st.st_size);
        _exit(0);
    } else {
        printf("Parent output:\n");
        write(STDOUT_FILENO, addr, st.st_size);
    }

    munmap(addr, st.st_size);
    close(fd);
    return 0;
}
