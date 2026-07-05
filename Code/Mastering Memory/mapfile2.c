#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    int fd = open("test.txt", O_RDWR);
    struct stat st;
    fstat(fd, &st);

    char *addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
 //   char *addr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    printf("Before write: %.12s\n", addr);
    getchar(); 
    printf("Before write: %.12s\n", addr);
    getchar(); 
    addr[0] = 'X';   // trigger copy-on-write
    printf("After write:  %.12s\n", addr);
    getchar(); 
}