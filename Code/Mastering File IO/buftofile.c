#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    const char *msg = "Hello from vmsplice!\n";
    size_t len = strlen(msg);

    struct iovec iov;
    iov.iov_base = (void *)msg;
    iov.iov_len  = len;

    ssize_t n = vmsplice(pipefd[1], &iov, 1, SPLICE_F_GIFT);
    if (n == -1) {
        perror("vmsplice");
        exit(EXIT_FAILURE);
    }

    int fd_out = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    ssize_t m = splice(pipefd[0], NULL, fd_out, NULL, n, 0);
    if (m == -1) {
        perror("splice");
        exit(EXIT_FAILURE);
    }

    close(fd_out);
    close(pipefd[0]);
    close(pipefd[1]);

    printf("Wrote %zd bytes to out.txt using vmsplice + splice\n", m);
    return 0;
}
