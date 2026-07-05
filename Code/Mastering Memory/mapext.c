#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{
    char *addr;
    int fd, i, length;
    struct stat file;
    int page;

    if (argc < 2) {
        printf ("\nUsage: %s filename", argv[0]);
        return 1;
    }

    /* Get the page size */
    page = sysconf (_SC_PAGE_SIZE);
    printf ("\nPAGESIZE: %d", page);

    /* Open the file */
    fd = open (argv[1], O_RDWR);
    if (fd == -1) {
        perror ("open");
        exit(EXIT_FAILURE);
    }

    /* Get the file size */
    if (fstat (fd, &file) == -1)
        perror ("fstat");

    length = file.st_size;
    size_t allocated = (length + page - 1) & ~(page - 1);  
    size_t pages = allocated / page;


    /* Map the file */
    addr=mmap(NULL, file.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror ("mmap");
        close(fd);
        return 1;
    }

    printf ("\nFile sizei: %d", file.st_size);
    printf ("\nMapping of %s created", argv[1]);
    printf ("\nAllocated pages = %d", pages);
    printf ("\n\n-- Pages addresses range --\n");
    printf ("%p - %p", &addr[0], &addr[page-1]);
    printf ("\n\nMapped file bytes: %td", &addr[length] - &addr[0]);
    printf ("\n\n-- File addresses range --\n");
    printf ("%p - %p", &addr[0], &addr[length-1]);
    printf ("\n\n-- Access memory not mapped to file -- \naddress: 0x%p \nvalue: %d", &addr[length], addr[length]);
    
    /* Increase the file size and remap */
    printf ("\n\n-- Increase file size -- \n");

    lseek (fd, file.st_size, SEEK_SET);
    if ((write (fd, "DATAS", 5)) == -1)
        perror ("write");
    msync(addr, file.st_size, MS_SYNC);

    /* Get the new file size */
    if (fstat (fd, &file) == -1)
        perror ("fstat");
    length = file.st_size;

    printf ("New file size: %ld", (long) file.st_size);
    printf ("\n\n-- File contents -- \n");
    write (STDOUT_FILENO, addr, file.st_size);

    printf ("\n\n-- File addresses range --\n");
    printf ("0x%p - 0x%p", &addr[0], &addr[length-1]);
    printf ("\n\n-- Access memory not mapped to file -- \naddress: 0x%p \nvalue: %d", &addr[length], addr[length]);

    /* Try to access beyond the mapping */
    printf ("\n\n-- Access beyond the mapping --");
    printf ("\nLast address of mapping: %p \nNext address contents: %d", &addr[page-1], addr[page+1]);

    /* Wait for user input */
    printf("\n\nEnter something to quit: ");
    char a;
    scanf("%c", &a);

    /* Clean up */
    munmap (addr, file.st_size);
    close (fd);
    return 0;
}