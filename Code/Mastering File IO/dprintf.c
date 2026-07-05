#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int a = 42;
    float b = 3.14;
    char *s = "Hello dprintf";

    dprintf(fd, "Integer: %d\n", a);
    dprintf(fd, "Float: %.2f\n", b);
    dprintf(fd, "String: %s\n", s);

    close(fd);

    printf("Data written to output.txt using dprintf()\n");
    return 0;
}