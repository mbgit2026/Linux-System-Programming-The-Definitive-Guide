#include <stdio.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd;
    struct iovec iov[3];
    ssize_t written, total = 0;

    fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char str1[] = "Hello, ";
    char str2[] = "world";
    char str3[] = "!\n";

    iov[0].iov_base = str1;
    iov[0].iov_len  = strlen(str1);
    total += iov[0].iov_len;

    iov[1].iov_base = str2;
    iov[1].iov_len  = strlen(str2);
    total += iov[1].iov_len;

    iov[2].iov_base = str3;
    iov[2].iov_len  = strlen(str3);
    total += iov[2].iov_len;

    written = writev(fd, iov, 3);
    if (written == -1) {
        perror("writev");
        close(fd);
        return 1;
    }

    if (written < total)
        fprintf(stderr, "Warning: only %zd of %zd bytes written\n", written, total);

    printf("Total requested bytes: %zd; bytes written: %zd\n", total, written);

    close(fd);
    return 0;
}


