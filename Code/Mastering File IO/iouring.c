#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <liburing.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

int main() {

    struct io_uring_params params;
    memset(&params, 0, sizeof(params));

    unsigned entries = 64;  
    void *sq, *cq;

    int ring_fd = io_uring_setup(entries, &params);
    if (ring_fd < 0) {
        perror("io_uring_setup");
        exit(EXIT_FAILURE);
    }

    printf("io_uring instance created. fd = %d\n", ring_fd);

    size_t sq_ring_sz = params.sq_off.array + params.sq_entries * sizeof(__u32);
    sq = mmap(0, sq_ring_sz, PROT_READ| PROT_WRITE, MAP_SHARED|MAP_POPULATE, ring_fd, IORING_OFF_SQ_RING);

    if (sq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    size_t cq_ring_sz = params.cq_off.cqes + params.cq_entries * sizeof(struct io_uring_cqe);
    cq = mmap(0, cq_ring_sz, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, ring_fd, IORING_OFF_CQ_RING);

    if (cq == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    struct io_uring_sqe *sqes = mmap(0, params.sq_entries * sizeof(struct io_uring_sqe), PROT_READ|	
    			                PROT_WRITE, MAP_SHARED|MAP_POPULATE, ring_fd, IORING_OFF_SQES);

    if (sqes == MAP_FAILED) {
        perror("mmap sqes");
        close(ring_fd);
        exit(EXIT_FAILURE);
    }

     // SQ ring pointers
    unsigned *sq_head = sq + params.sq_off.head;
    unsigned *sq_tail = sq + params.sq_off.tail;
    unsigned *sq_array = sq + params.sq_off.array;

    // CQ ring pointers
    unsigned *cq_head = cq + params.cq_off.head;
    unsigned *cq_tail = cq + params.cq_off.tail;
    struct io_uring_cqe *cqes = cq + params.cq_off.cqes;


     // Prepare write request
    const char msg[] = "Hello io_uring!\n";
    int idx = *sq_tail & (params.sq_entries - 1);

    memset(&sqes[idx], 0, sizeof(sqes[idx]));
    sqes[idx].opcode = IORING_OP_WRITE;
    sqes[idx].fd = 1; 
    sqes[idx].addr = (unsigned long)msg;
    sqes[idx].len = sizeof(msg) - 1;
    sqes[idx].off = 0;
    sq_array[idx] = idx;

    *sq_tail = *sq_tail + 1;
    if (io_uring_enter(ring_fd, 1, 0, 0, NULL) < 0) {
        perror("io_uring_enter submit");
        exit(EXIT_FAILURE);
    }

    if (io_uring_enter(ring_fd, 0, 1, IORING_ENTER_GETEVENTS, NULL) < 0) {
        perror("io_uring_enter wait");
        exit(EXIT_FAILURE);
    }

    struct io_uring_cqe cqe = cqes[*cq_head & (params.cq_entries - 1)];
    if (cqe.res < 0) {
        fprintf(stderr, "Write failed: %s\n", strerror(-cqe.res));
    } else {
        printf("Write completed, %d bytes written.\n", cqe.res);
    }

    *cq_head = *cq_head + 1;
    munmap(sq, sq_ring_sz);
    munmap(cq, cq_ring_sz);
    munmap(sqes, params.sq_entries * sizeof(struct io_uring_sqe));
    close(ring_fd);
    return 0;
}
