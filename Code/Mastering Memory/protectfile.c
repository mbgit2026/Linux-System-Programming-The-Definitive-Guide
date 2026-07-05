#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <crypt.h>

#define PASSWORD_HASH "$6$8P3pOhYCHJhXdafx$7PyvgJMd1pZc.OGibhG3//V0fLm3btUKeUpMTzzPTpPidzEGO39.OTo.WrR9ztYMjkebCfB6uGACbqM0NZRa71"

int main() {
    
    const char *filename = "newfile.txt";

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
        err(EXIT_FAILURE, "open");

    struct stat st;
    if (fstat(fd, &st) == -1)
        err(EXIT_FAILURE, "fstat");
    size_t filesize = st.st_size;

    void *map = mmap(NULL, filesize, PROT_NONE, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
        err(EXIT_FAILURE, "mmap");

    close(fd);  
    unlink(filename);
    printf("File is mapped but inaccessible until password is entered.\n");

    char input[128];

    printf("Enter password: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    /* Lock password in RAM */
    mlock(input, sizeof(input));

    char *hash = crypt(input, PASSWORD_HASH);

    /* Zero password immediately */
    explicit_bzero(input, sizeof(input));

    if (!hash || strcmp(hash, PASSWORD_HASH) != 0) {
        fprintf(stderr, "Incorrect password\n");
        munmap(map, filesize);
        return 1;
    }

    if (mprotect(map, filesize, PROT_READ) == -1)
        err(EXIT_FAILURE, "mprotect");

    printf("\nAccess granted! File contents:\n\n");
    fwrite(map, 1, filesize, stdout);
    putchar('\n');

     if (munmap(map, filesize) == -1)
        perror("munmap failed");

    return 0;
}
