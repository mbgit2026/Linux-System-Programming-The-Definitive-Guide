#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(void)
{
    int fd, ret;

    fd = open("/tmp", O_TMPFILE | O_RDWR | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    write(fd, "Linux", 5);

    ret = linkat(fd, "", AT_FDCWD, "/tmp/filetemp.txt", AT_EMPTY_PATH);
    if (ret != 0)
        perror("linkat");

    close(fd);
    return 0;
}
