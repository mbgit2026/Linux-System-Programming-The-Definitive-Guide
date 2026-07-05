#define _GNU_SOURCE
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main() {
    const char *filename = "output.txt";

    // Open file in append mode
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct io_uring ring;
    if (io_uring_queue_init(4, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        close(fd);
        return 1;
    }

    // Three simple buffers to write
    char *bufs[3] = {
        "Hello, first write!\n",
        "Second write here!\n",
        "Third write completed!\n"
    };

    // Submit three writes asynchronously
    for (int i = 0; i < 3; i++) {
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if (!sqe) {
            fprintf(stderr, "Failed to get SQE\n");
            break;
        }
        // offset = -1 tells io_uring to honor O_APPEND
        io_uring_prep_write(sqe, fd, bufs[i], strlen(bufs[i]), -1);
        io_uring_sqe_set_data(sqe, bufs[i]);
    }

    io_uring_submit(&ring);

    // Wait for completions
    for (int i = 0; i < 3; i++) {
        struct io_uring_cqe *cqe;
        if (io_uring_wait_cqe(&ring, &cqe) < 0) {
            perror("io_uring_wait_cqe");
            break;
        }
        printf("Write completed: %d bytes\n", cqe->res);
        io_uring_cqe_seen(&ring, cqe);
    }

    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}
