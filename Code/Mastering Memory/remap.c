#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

void cleanup(void *addr, off_t size, int fd, int fd2, void *buf)
{
    if (buf) free(buf);
    if (fd2 >= 0) close(fd2);
    if (fd >= 0) close(fd);
    if (addr && size > 0) munmap(addr, size);
}

int main (int argc, char *argv[]) 
{
    char *addr = NULL;
    char *buf = NULL;
    int fd = -1, fd2 = -1;
    off_t old_size = 0, new_size = 0;
    struct stat file, file2;

    if (argc < 2) {
        printf ("\nUsage: %s filename", argv[0]);
        return 1;
    }

    fd = open (argv[1], O_RDWR);
    if (fd == -1) {
        perror ("open");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    if (fstat (fd, &file) == -1) {
        perror ("fstat");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }
        

    if (file.st_size == 0) {
        printf("\nFile is empty, exit...");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    old_size = file.st_size;

    addr=mmap(NULL, file.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror ("mmap");
        cleanup(NULL, 0, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }
    
    fd2 = open("count2.txt", O_RDONLY);
    if (fd2 == -1) {
        perror("open count2.txt");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    if (fstat(fd2, &file2) == -1)  {
        perror("fstat");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    buf = malloc(file2.st_size);
    if (!buf) {
        perror("malloc");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    ssize_t r = read(fd2, buf, file2.st_size);
    if (r != file2.st_size) {
        perror("read");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    close(fd2);
    fd2 = -1;

    if (lseek(fd, 0, SEEK_END) == -1) {
        perror("lseek");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    if ((write (fd, buf, file2.st_size)) == -1) {
        perror ("write");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    free(buf);
    buf = NULL;

    if (fstat (fd, &file) == -1) {
        perror ("fstat");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }

    new_size = file.st_size;

   /* void *new_addr = mremap(addr, old_size, new_size, MREMAP_MAYMOVE);
    if (new_addr == MAP_FAILED) {
        perror("mremap");
        cleanup(addr, old_size, fd, fd2, buf);
        exit(EXIT_FAILURE);
    }
    addr = new_addr;
    */

    write (STDOUT_FILENO, addr, new_size);
    cleanup(addr, new_size, fd, fd2, buf);
    return 0;
}