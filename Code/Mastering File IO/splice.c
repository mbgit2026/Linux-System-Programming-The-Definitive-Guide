#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <src_file> <dst_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        perror("open source");
        exit(EXIT_FAILURE);
    }

    int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("open destination");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        close(fd_in);
        close(fd_out);
        exit(EXIT_FAILURE);
    }

    ssize_t n;
    while (1) {
        n = splice(fd_in, NULL, pipefd[1], NULL, BUF_SIZE, 0);
        if (n == -1) {
            perror("splice read");
            break;
        }
        if (n == 0) break; 

        ssize_t written = splice(pipefd[0], NULL, fd_out, NULL, n, 0);
        if (written == -1) {
            perror("splice write");
            break;
        }
    }

    close(pipefd[0]);
    close(pipefd[1]);
    close(fd_in);
    close(fd_out);

    return 0;
}
