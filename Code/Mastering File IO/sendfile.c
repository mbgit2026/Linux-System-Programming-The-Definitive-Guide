#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <src_file> <dst_file> <offset_src> <offset_dst> <bytes>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd_src, fd_dst;
    off_t offset_src = atoll(argv[3]);
    off_t offset_dst = atoll(argv[4]);
    size_t bytes = atoll(argv[5]);

    fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        perror("open source");
        exit(EXIT_FAILURE);
    }

    fd_dst = open(argv[2], O_WRONLY | O_CREAT, 0644);
    if (fd_dst == -1) {
        perror("open destination");
        close(fd_src);
        exit(EXIT_FAILURE);
    }

    if (lseek(fd_dst, offset_dst, SEEK_SET) == (off_t)-1) {
        perror("lseek destination");
        close(fd_src);
        close(fd_dst);
        exit(EXIT_FAILURE);
    }

    ssize_t sent = sendfile(fd_dst, fd_src, &offset_src, bytes);
    if (sent == -1) {
        perror("sendfile");
        close(fd_src);
        close(fd_dst);
        exit(EXIT_FAILURE);
    }

    printf("Copied %zd bytes from '%s' (offset %lld) to '%s' (offset %lld)\n",
           sent, argv[1], (long long) offset_src, argv[2], (long long) offset_dst);

    close(fd_src);
    close(fd_dst);
    return 0;
}
