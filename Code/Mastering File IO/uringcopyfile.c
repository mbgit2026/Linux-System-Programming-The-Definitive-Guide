/* NOT INSERTED INTO THE BOOK */

#define _GNU_SOURCE
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

typedef struct {
    char *buf;
    off_t offset;
    int size;       
    int is_read;    
} io_req;

void err_exit(const char *call, struct io_uring *ring) {
    perror(call);
    if (ring) io_uring_queue_exit(ring);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <src> <dst> <concurrency> <bufsize>\n", argv[0]);
        return 1;
    }

    const char *src = argv[1];
    const char *dst = argv[2];
    int concurrency = atoi(argv[3]);
    int bufsize = atoi(argv[4]);

    int fdin = open(src, O_RDONLY);
    if (fdin < 0) { perror("open input"); return 1; }

    int fdout = open(dst, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fdout < 0) { perror("open output"); close(fdin); return 1; }

    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) page_size = 4096;

    struct io_uring ring;
    int entries = concurrency * 2 + 8;
    if (io_uring_queue_init(entries, &ring, 0) < 0)
        err_exit("io_uring_queue_init", &ring);

    off_t file_size = lseek(fdin, 0, SEEK_END);
    lseek(fdin, 0, SEEK_SET);

    if (file_size == 0) {
        printf("Source file empty, nothing to copy.\n");
        io_uring_queue_exit(&ring);
        close(fdin);
        close(fdout);
        return 0;
    }

    io_req *reqs = malloc(sizeof(io_req) * concurrency);
    if (!reqs) err_exit("malloc reqs", &ring);

    for (int i = 0; i < concurrency; ++i) {
        if (posix_memalign((void **)&reqs[i].buf, (size_t)page_size, bufsize) != 0) {
            perror("posix_memalign");
            for (int j = 0; j < i; ++j) free(reqs[j].buf);
            free(reqs);
            err_exit("posix_memalign", &ring);
        }
        memset(reqs[i].buf, 0, bufsize);
    }

    off_t next_offset = 0;      
    off_t finished_bytes = 0;   
    int inflight = 0; 
    int tail = 0;               

    while (finished_bytes < file_size) {
        while (inflight < concurrency && next_offset < file_size) {
            int idx = tail; // reuse the slot
            struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
            if (!sqe) break;

            int this_size = bufsize;
            if (next_offset + this_size > file_size)
                this_size = (int)(file_size - next_offset);

            reqs[idx].offset = next_offset;
            reqs[idx].size = this_size;
            reqs[idx].is_read = 1;

            io_uring_prep_read(sqe, fdin, reqs[idx].buf, this_size, reqs[idx].offset);
            io_uring_sqe_set_data(sqe, &reqs[idx]);

            next_offset += this_size;
            inflight++;
            tail = (tail + 1) % concurrency;
        }

        if (io_uring_submit(&ring) < 0)
            err_exit("io_uring_submit", &ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) err_exit("io_uring_wait_cqe", &ring);

        io_req *r = (io_req *)io_uring_cqe_get_data(cqe);
        if (!r) {
            fprintf(stderr, "Received cqe with NULL data\n");
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        if (cqe->res < 0) {
            fprintf(stderr, "Async IO failed at offset %ld: %s\n", r->offset, strerror(-cqe->res));
            inflight--;
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        if (r->is_read) {
            int nread = cqe->res;
            if (nread == 0) {
                inflight--;
                io_uring_cqe_seen(&ring, cqe);
                continue;
            }
            struct io_uring_sqe *wsqe = io_uring_get_sqe(&ring);
            if (!wsqe) err_exit("get_sqe for write", &ring);

            io_uring_prep_write(wsqe, fdout, r->buf, nread, r->offset);
            r->is_read = 0;
            r->size = nread; 
            io_uring_sqe_set_data(wsqe, r);
            if (io_uring_submit(&ring) < 0)
                err_exit("io_uring_submit(write)", &ring);
        } else {
            int nw = cqe->res;
            finished_bytes += nw;
            inflight--;
        }

        io_uring_cqe_seen(&ring, cqe);
    }

    for (int i = 0; i < concurrency; ++i) free(reqs[i].buf);
    free(reqs);

    io_uring_queue_exit(&ring);
    close(fdin);
    close(fdout);

    printf("Copy completed successfully: %ld bytes\n", (long)finished_bytes);
    return 0;
}
