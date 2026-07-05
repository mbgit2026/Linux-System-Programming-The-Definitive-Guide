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

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <n>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numreq = atoi(argv[1]);

    struct io_uring_params params;
    memset(&params, 0, sizeof(params));
    int i;

    int ring_fd = io_uring_setup(numreq, &params);
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


    char files[numreq][16]; 
    for (i = 0; i < numreq; i++) {
        snprintf(files[i], sizeof(files[i]), "file%d", i+1);
    }

    int fds[numreq];
    for (i = 0; i<numreq; i++) {
        fds[i] = open(files[i], O_CREAT | O_WRONLY | O_DIRECT , 0644);
        if (fds[i] < 0) {
            perror("open");
            exit(1);
        }
    }

    if (io_uring_register(ring_fd, IORING_REGISTER_FILES, fds, numreq) < 0) {
        perror("io_uring_register_files");
        exit(1);
    }

    void *msg;
    posix_memalign(&msg, 32768, 32768);  
    memset(msg, 'A', 32768);
    
    int idxs[numreq];
    for (i=0; i<numreq; i++) {
        int idx = *sq_tail & (params.sq_entries - 1);
        memset(&sqes[idx], 0, sizeof(sqes[idx]));
        sqes[idx].opcode = IORING_OP_WRITE;
        sqes[idx].fd = i;   
        sqes[idx].addr = (unsigned long)msg;
        sqes[idx].len = strlen(msg);
        sqes[idx].off = 0;
        sqes[idx].flags = IOSQE_FIXED_FILE;
        sqes[idx].user_data = 1000 + i;  
        sq_array[idx] = idx;
        idxs[i] = idx;
        *sq_tail += 1;
    }


    if (io_uring_enter(ring_fd, numreq, 0, 0, NULL) < 0) {
        perror("io_uring_enter submit");
        exit(1);
    }

    int completed = 0;
    while (completed < numreq) {
        if (*cq_head != *cq_tail) {
            struct io_uring_cqe cqe = cqes[*cq_head & (params.cq_entries - 1)];
            if (cqe.res < 0) {
                fprintf(stderr, "Write failed: %s\n", strerror(-cqe.res));
            }/* else {
                printf("Write completed (user_data=%llu): %d bytes\n",
                       (unsigned long long)cqe.user_data,
                       cqe.res);
            }*/
            *cq_head += 1;
            completed++;
        } else {
            if (io_uring_enter(ring_fd, 0, 1, IORING_ENTER_GETEVENTS, NULL) < 0) {
                perror("io_uring_enter wait");
                exit(1);
            }
        }
    }

    io_uring_register(ring_fd, IORING_UNREGISTER_FILES, NULL, 0);
  
    for (i=0; i<numreq; i++)
        close(fds[i]);
    
    munmap(sq_ptr, sq_ring_sz);
    munmap(cq_ptr, cq_ring_sz);
    munmap(sqes, params.sq_entries * sizeof(struct io_uring_sqe));
    close(ring_fd);
    return 0;
}