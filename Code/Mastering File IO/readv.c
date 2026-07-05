#include <stdio.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define LEN 4

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

    char str[LEN] = {0}, str2[LEN] = {0}, str3[LEN] = {0};
    struct iovec iov[3];

    iov[0].iov_base = str;
    iov[0].iov_len  = LEN;
    iov[1].iov_base = str2;
    iov[1].iov_len  = LEN;
    iov[2].iov_base = str3;
    iov[2].iov_len  = LEN;

    ssize_t readed = readv(fd, iov, 3);
    if (readed == -1) {
        perror("readv");
        close(fd);
        return 1;
    }

    printf("\nBuffer 1 = %.*s", LEN, str);
    printf("\nBuffer 2 = %.*s", LEN, str2);
    printf("\nBuffer 3 = %.*s", LEN, str3);

    printf("\n\nTotal bytes read: %zd\n", readed);

    close(fd);
    return 0;
}

