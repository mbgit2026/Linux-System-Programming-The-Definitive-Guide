#define _GNU_SOURCE
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BS  128  

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <file> <concurrent_writes> <write_count>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int concurrent_writes = atoi(argv[2]);
    int write_count = atoi(argv[3]);

    if (concurrent_writes <= 0 || write_count <= 0) {
        fprintf(stderr, "concurrent_writes and write_count must be positive integers\n");
        return 1;
    }

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct io_uring ring;
    if (io_uring_queue_init(concurrent_writes * 2, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        close(fd);
        return 1;
    }


    char *buf[write_count];
    for (int i = 0; i < write_count; i++) {
        buf[i] = malloc(BS);
        if (!buf[i]) {
            perror("malloc");
            for (int j = 0; j < i; j++) 
                free(buf[j]);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        memset(buf[i], 'A', BS); 
    }

    int writes_submitted = 0;
    int writes_completed = 0;
    off_t offset = 0;

    while (writes_completed < write_count) {
        while (writes_submitted - writes_completed < concurrent_writes && writes_submitted < write_count) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            if (!sqe) {
                fprintf(stderr, "Failed to get SQE\n");
                break;
            }
            io_uring_prep_write(sqe, fd, buf[writes_submitted], BS, offset);
            io_uring_sqe_set_data(sqe, buf[writes_submitted]);  
            offset += BS;
            writes_submitted++;
        }

        // Submit pending writes
        if (io_uring_submit(&ring) < 0) {
            perror("io_uring_submit");
            break;
        }

        // Wait for completions
        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            perror("io_uring_wait_cqe");
            break;
        }

        if (cqe->res < 0) {
            fprintf(stderr, "Async write failed: %s\n", strerror(-cqe->res));
        } else {
            printf("Write completed: %d bytes\n", cqe->res);
        }

        io_uring_cqe_seen(&ring, cqe);
        writes_completed++;
    }

    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}
