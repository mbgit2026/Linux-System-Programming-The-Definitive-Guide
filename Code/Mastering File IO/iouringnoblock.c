#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <liburing.h>

int main() {

    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    unsigned entries = 8;

    int ring_fd = io_uring_setup(entries, &params);
    if (ring_fd < 0) {
        perror("io_uring_setup");
        exit(1);
    }

    size_t sq_ring_sz = params.sq_off.array + params.sq_entries * sizeof(__u32);
    void *sq_ptr = mmap(0, sq_ring_sz, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQ_RING);
    if (sq_ptr == MAP_FAILED) {
        perror("mmap sq");
        exit(1);
    }

    size_t cq_ring_sz = params.cq_off.cqes + params.cq_entries * sizeof(struct io_uring_cqe);
    void *cq_ptr = mmap(0, cq_ring_sz, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_CQ_RING);
    if (cq_ptr == MAP_FAILED) {
        perror("mmap cq");
        exit(1);
    }

    struct io_uring_sqe *sqes = mmap(0, params.sq_entries * sizeof(struct io_uring_sqe),
                                     PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
                                     ring_fd, IORING_OFF_SQES);
    if (sqes == MAP_FAILED) {
        perror("mmap sqes");
        exit(1);
    }

    unsigned *sq_head  = sq_ptr + params.sq_off.head;
    unsigned *sq_tail  = sq_ptr + params.sq_off.tail;
    unsigned *sq_array = sq_ptr + params.sq_off.array;

    unsigned *cq_head = cq_ptr + params.cq_off.head;
    unsigned *cq_tail = cq_ptr + params.cq_off.tail;
    struct io_uring_cqe *cqes = cq_ptr + params.cq_off.cqes;

    int fd1 = open("file1.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    int fd2 = open("file2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd1 < 0 || fd2 < 0) {
        perror("open");
        exit(1);
    }

    int fds[2] = { fd1, fd2 };
    if (io_uring_register(ring_fd, IORING_REGISTER_FILES, fds, 2) < 0) {
        perror("io_uring_register_files");
        exit(1);
    }

    const char msg1[] = "Hello from file1!\n";
    const char msg2[] = "Hello from file2!\n";

    int idx1 = *sq_tail & (params.sq_entries - 1);
    memset(&sqes[idx1], 0, sizeof(sqes[idx1]));
    sqes[idx1].opcode = IORING_OP_WRITE;
    sqes[idx1].fd = 0; // registered file index 0 -> file1
    sqes[idx1].addr = (unsigned long)msg1;
    sqes[idx1].len = strlen(msg1);
    sqes[idx1].off = 0;
    sqes[idx1].flags = IOSQE_FIXED_FILE;
    sqes[idx1].user_data = 1001; // tag it
    sq_array[idx1] = idx1;
    *sq_tail += 1;

    int idx2 = *sq_tail & (params.sq_entries - 1);
    memset(&sqes[idx2], 0, sizeof(sqes[idx2]));
    sqes[idx2].opcode = IORING_OP_WRITE;
    sqes[idx2].fd = 1; // registered file index 1 -> file2
    sqes[idx2].addr = (unsigned long)msg2;
    sqes[idx2].len = strlen(msg2);
    sqes[idx2].off = 0;
    sqes[idx2].flags = IOSQE_FIXED_FILE;
    sqes[idx2].user_data = 1002; // tag it
    sq_array[idx2] = idx2;
    *sq_tail += 1;

    if (io_uring_enter(ring_fd, 2, 0, 0, NULL) < 0) {
        perror("io_uring_enter submit");
        exit(1);
    }

    int completed = 0;
    while (completed < 2) {
        if (*cq_head != *cq_tail) {
            // At least one CQE is ready
            struct io_uring_cqe cqe = cqes[*cq_head & (params.cq_entries - 1)];
            if (cqe.res < 0) {
                fprintf(stderr, "Write failed (user_data=%llu): %s\n",
                        (unsigned long long)cqe.user_data,
                        strerror(-cqe.res));
            } else {
                printf("Write completed (user_data=%llu): %d bytes\n",
                       (unsigned long long)cqe.user_data,
                       cqe.res);
            }
            *cq_head += 1;
            completed++;
        } else {
            // Nothing ready yet → block until something completes
            if (io_uring_enter(ring_fd, 0, 1, IORING_ENTER_GETEVENTS, NULL) < 0) {
                perror("io_uring_enter wait");
                exit(1);
            }
        }
    }

    io_uring_register(ring_fd, IORING_UNREGISTER_FILES, NULL, 0);
    close(fd1);
    close(fd2);
    munmap(sq_ptr, sq_ring_sz);
    munmap(cq_ptr, cq_ring_sz);
    munmap(sqes, params.sq_entries * sizeof(struct io_uring_sqe));
    close(ring_fd);
    return 0;
}