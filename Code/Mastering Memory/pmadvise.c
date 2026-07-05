#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/wait.h>

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    size_t len = page_size;

    char *buffer = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < len; i++)
        buffer[i] = 'a';

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /* -------- Child -------- */
        printf("Child: first byte before advice = %c\n", buffer[0]);
        fflush(stdout);

        /* Wait for parent to send the advice */
        sleep(2);

        /* Access again */
        printf("Child: first byte after advice  = %c\n", buffer[0]);
        unsigned char vec;
        mincore(buffer, len, &vec);
        printf("Child: resident = %s\n", (vec & 1) ? "yes" : "no");
        munmap(buffer, len);
        exit(0);
    }

    /* -------- Parent -------- */
    sleep(1);

    int pidfd = syscall(SYS_pidfd_open, pid, 0);
    if (pidfd == -1) {
        perror("pidfd_open");
        return EXIT_FAILURE;
    }

    struct iovec iov = {
        .iov_base = buffer,
        .iov_len  = len
    };

    ssize_t ret = process_madvise(pidfd, &iov, 1, MADV_COLD, 0);
    if (ret == -1) {
        perror("process_madvise");
    } else {
        printf("MADV_PAGEOUT sent\n");
    }

    close(pidfd);
    wait(NULL);
    munmap(buffer, len);
    return EXIT_SUCCESS;
}
