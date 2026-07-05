#define _GNU_SOURCE
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define QD  1          // Queue depth
#define BS  1024       // Buffer size

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // Open the file for reading
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // Initialize io_uring
    struct io_uring ring;
    if (io_uring_queue_init(QD, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        close(fd);
        return 1;
    }

    // Allocate buffer
    char *buf = malloc(BS);
    if (!buf) {
        perror("malloc");
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Get a submission queue entry (SQE)
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        fprintf(stderr, "Failed to get SQE\n");
        free(buf);
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Prepare a read request
    io_uring_prep_read(sqe, fd, buf, BS, 0);

    // Submit the request
    if (io_uring_submit(&ring) < 0) {
        perror("io_uring_submit");
        free(buf);
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Wait for completion
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring, &cqe) < 0) {
        perror("io_uring_wait_cqe");
        free(buf);
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    // Check result
    if (cqe->res < 0) {
        fprintf(stderr, "Async read failed: %s\n", strerror(-cqe->res));
    } else {
        printf("Read %d bytes:\n%.*s\n", cqe->res, cqe->res, buf);
    }

    // Mark this completion as seen
    io_uring_cqe_seen(&ring, cqe);

    // Cleanup
    free(buf);
    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}
