#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main (int argc, char *argv[])
{
    char *addr, *addr2;
    int fd;
    struct stat file;
    void *fixaddr = (void *) 0x7ffe559e9000;

    fd = open ("newfile.txt", O_RDONLY);
    if (fd == -1)
        perror ("open");

    if (fstat (fd, &file) == -1)
        perror ("fstat");

    addr=mmap(fixaddr, file.st_size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE|MAP_FIXED,-1, 0);
    if (addr == MAP_FAILED)
        perror ("mmap");

    addr2=mmap(NULL, file.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
     if (addr == MAP_FAILED)
        perror ("mmap");

    memcpy (addr, addr2, file.st_size);
    write (STDOUT_FILENO, addr, file.st_size);
    sleep (200);

    close (fd);
    munmap (addr, file.st_size);
    munmap (addr2, file.st_size);
    return 0;
}