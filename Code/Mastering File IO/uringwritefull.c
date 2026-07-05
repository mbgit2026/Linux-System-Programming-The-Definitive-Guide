#define _GNU_SOURCE
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BS 64

typedef struct {
    char *buf;      // pointer to buffer
    off_t offset;   // offset in file
} write_req_t;

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

    // Allocate buffers and requests
    write_req_t *reqs = malloc(sizeof(write_req_t) * write_count);
    if (!reqs) {
        perror("malloc");
        io_uring_queue_exit(&ring);
        close(fd);
        return 1;
    }

    for (int i = 0; i < write_count; i++) {
        reqs[i].buf = malloc(BS);
        if (!reqs[i].buf) {
            perror("malloc");
            for (int j = 0; j < i; j++) 
                free(reqs[j].buf);
            free(reqs);
            io_uring_queue_exit(&ring);
            close(fd);
            return 1;
        }
        memset(reqs[i].buf, 'A', BS); 
        reqs[i].offset = i * BS;
    }

    int submitted = 0;
    int completed = 0;

    while (completed < write_count) {
        // Submit as many writes as allowed by concurrency
        while (submitted - completed < concurrent_writes && submitted < write_count) {
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            if (!sqe) break;

            io_uring_prep_write(sqe, fd, reqs[submitted].buf, BS, reqs[submitted].offset);
            io_uring_sqe_set_data(sqe, &reqs[submitted]);
            submitted++;
        }

        if (io_uring_submit(&ring) < 0) {
            perror("io_uring_submit");
            break;
        }

        // Process completions as they become available
        struct io_uring_cqe *cqe;
        while (io_uring_peek_cqe(&ring, &cqe) == 0) {
            write_req_t *r = (write_req_t *)io_uring_cqe_get_data(cqe);
            if (cqe->res < 0) {
                fprintf(stderr, "Async write failed: %s\n", strerror(-cqe->res));
            } else {
                printf("Write completed at offset %ld: %d bytes \ndata: %s\n", r->offset, cqe->res, r->buf);
            }
            io_uring_cqe_seen(&ring, cqe);
            completed++;
        }
    }

    
    for (int i = 0; i < write_count; i++)
         free(reqs[i].buf);
    free(reqs);
    
    io_uring_queue_exit(&ring);
    close(fd);
    return 0;
}
