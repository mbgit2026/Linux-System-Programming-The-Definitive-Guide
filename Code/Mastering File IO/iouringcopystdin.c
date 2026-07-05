#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <liburing.h>
#include <errno.h>

#define QUEUE_DEPTH 1
#define BLOCK_SZ    1024

int ring_fd;
unsigned *sring_tail, *sring_mask, *sring_array, *cring_head, *cring_tail, *cring_mask;
struct io_uring_sqe *sqes;
struct io_uring_cqe *cqes;
char buff[BLOCK_SZ];
off_t offset;


int app_setup_uring(void) {

    struct io_uring_params p;
    void *sq_ptr, *cq_ptr;

    memset(&p, 0, sizeof(p));
    ring_fd = io_uring_setup(QUEUE_DEPTH, &p);
    if (ring_fd < 0) {
        perror("io_uring_setup");
        return 1;
    }

    int sring_sz = p.sq_off.array + p.sq_entries * sizeof(unsigned);
    int cring_sz = p.cq_off.cqes + p.cq_entries * sizeof(struct io_uring_cqe);

    sq_ptr = mmap(0, sring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_SQ_RING);
    if (sq_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    cq_ptr = mmap(0, cring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring_fd, IORING_OFF_CQ_RING);
    if (cq_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
      
    sring_tail = sq_ptr + p.sq_off.tail;
    sring_mask = sq_ptr + p.sq_off.ring_mask;
    sring_array = sq_ptr + p.sq_off.array;

    sqes = mmap(0, p.sq_entries * sizeof(struct io_uring_sqe), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
                    ring_fd, IORING_OFF_SQES);
    if (sqes == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
   
    cring_head = cq_ptr + p.cq_off.head;
    cring_tail = cq_ptr + p.cq_off.tail;
    cring_mask = cq_ptr + p.cq_off.ring_mask;
    cqes = cq_ptr + p.cq_off.cqes;
    return 0;
}


int read_from_cq() {
    struct io_uring_cqe *cqe;
    unsigned head;

    /* Read barrier */
    //head = io_uring_smp_load_acquire(cring_head);
    head = *cring_head;
   
    if (head == *cring_tail)
        return -1;

    cqe = &cqes[head & (*cring_mask)];
    if (cqe->res < 0)
        fprintf(stderr, "Error: %s\n", strerror(abs(cqe->res)));

    head++;

    /* Write barrier so that update to the head are made visible */
  //  io_uring_smp_store_release(cring_head, head);

    *cring_head = head;

    return cqe->res;
}


int submit_to_sq(int fd, int op) {
    unsigned index, tail;

    tail = *sring_tail;
    index = tail & *sring_mask;
    struct io_uring_sqe *sqe = &sqes[index];
 
    sqe->opcode = op;
    sqe->fd = fd;
    sqe->addr = (unsigned long) buff;
    if (op == IORING_OP_READ) {
        memset(buff, 0, sizeof(buff));
        sqe->len = BLOCK_SZ;
    }
    else {
        sqe->len = strlen(buff);
    }
    sqe->off = offset;

    sring_array[index] = index;
    tail++;

    /* Update the tail */
    //io_uring_smp_store_release(sring_tail, tail);

    *sring_tail = tail;

   
    int ret =  io_uring_enter(ring_fd, 1,1, IORING_ENTER_GETEVENTS, NULL);
    if(ret < 0) {
        perror("io_uring_enter");
        return -1;
    }

    return ret;
}
int main(int argc, char *argv[]) {
    int res;

    if(app_setup_uring()) {
        fprintf(stderr, "Unable to setup uring!\n");
        return 1;
    }

    while (1) {
        submit_to_sq(STDIN_FILENO, IORING_OP_READ);
        res = read_from_cq();
        if (res > 0) {
            submit_to_sq(STDOUT_FILENO, IORING_OP_WRITE);
            read_from_cq();
        } else if (res == 0) {
            break;
        }
        else if (res < 0) {
            fprintf(stderr, "Error: %s\n", strerror(abs(res)));
            break;
        }
        offset += res;
    }

    return 0;
}
