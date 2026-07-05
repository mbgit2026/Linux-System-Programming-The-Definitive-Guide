#define _GNU_SOURCE
#include <stdio.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

double now() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void initialize_aio(int aio_threads, int concurrent_req) {
    struct aioinit aiost;
    aiost.aio_threads = aio_threads;    
    aiost.aio_num = concurrent_req;  
    aiost.aio_idle_time = 1;  
    aiost.aio_reserved = 0;
    aio_init(&aiost);
}

struct aiocb Request (int fd, off_t offset, void * content, size_t len) 
{
    struct aiocb aio;
    memset(&aio, 0, sizeof(aio));      
    aio.aio_fildes = fd;
    aio.aio_offset = offset;
    aio.aio_buf = content;
    aio.aio_nbytes = len;
    aio.aio_reqprio = 0;
    aio.aio_sigevent.sigev_notify = SIGEV_NONE;
    aio.aio_lio_opcode = LIO_WRITE;
    return aio;
}

int main (int argc, char *argv[])
{
    if (argc != 6) {
        fprintf(stderr, "\nUsage: %s <file> <numreq> <threads> <concurrentreq> <reqsize>\n", argv[0]);
        return 1;
    }

    if (atoi(argv[2]) == 0) {
        printf("\nNo IO request to do, exiting...");
        return 0;
    }

    int fd, i;
    int numreq = atoi(argv[2]);
    int size = atoi(argv[5]) * 1024 * 1024;

    int rndfd = open("/dev/urandom", O_RDONLY);
    if (rndfd == -1) {
        perror("open /dev/urandom");
        close(fd);
        return 1;
    }
    void **buffers = calloc(numreq, sizeof(void *));
    if (!buffers) {
        perror("calloc");
        return 1;
    }

    for (int i = 0; i < numreq; i++) {
        buffers[i] = malloc(size);
        if (!buffers[i]) {
            perror("malloc");
            return 1;
        }
        ssize_t got = read(rndfd, buffers[i], size);
        if (got != size) {
            perror("read /dev/urandom");
            return 1;
        }
    }


    initialize_aio(atoi(argv[3]), atoi(argv[4]));

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct aiocb **op = calloc(numreq, sizeof(struct aiocb *));
    off_t offset = 0;
    for (i = 0; i < numreq; i++) {
        op[i] = calloc(1, sizeof(struct aiocb));
        *op[i] = Request(fd, offset, buffers[i], size);
        offset += size;
    }

    double start = now();

    if (lio_listio(LIO_NOWAIT, op, numreq, NULL) < 0) {
        perror("lio_listio");
        return 1;
    }
    
    int completed = 0;
    while (completed < numreq) {
        if (aio_suspend((const struct aiocb * const *)op, numreq, NULL) == -1) {
            if (errno == EINTR) continue; 
            perror("aio_suspend");
            break;
        }

        for (int i = 0; i < numreq; i++) {
            int err = aio_error(op[i]);
            if (err == 0) {
                ssize_t ret = aio_return(op[i]);
                printf("Request %d completed, wrote %zd bytes\n", i, ret);
                completed++;
            } else if (err == EINPROGRESS) {
                continue;
            } else {
                printf("Request %d failed: %s\n", i, strerror(err));
                completed++;
            }
        }
    }

    double end = now();
    printf("Total time: %.6f seconds\n", end - start);
  
    free(op);
    free(buffers);
    close(fd);
    return 0;
}
