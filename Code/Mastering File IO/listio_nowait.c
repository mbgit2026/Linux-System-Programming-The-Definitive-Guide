#include <stdio.h>
#include <aio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

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
    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <file>\n", argv[0]);
        return 1;
    }

    struct aiocb * op[2];
    int fd;
    char *str  = "Asynchronous";
    char *str2 = "Requests";

    fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct aiocb op1 = Request(fd, 0, str, strlen(str));
    struct aiocb op2 = Request(fd, strlen(str), str2, strlen(str2));

    op[0] = &op1;
    op[1] = &op2;

    if (lio_listio(LIO_NOWAIT, op, 2, NULL) < 0) {
        perror("lio_listio");
        return 1;
    }

    int completed = 0;
    while (completed < 2) {
        if (aio_suspend((const struct aiocb * const *)op, 2, NULL) == -1) {
            if (errno == EINTR) continue; 
            perror("aio_suspend");
            break;
        }

        for (int i = 0; i < 2; i++) {
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

    close(fd);
    return 0;
}
