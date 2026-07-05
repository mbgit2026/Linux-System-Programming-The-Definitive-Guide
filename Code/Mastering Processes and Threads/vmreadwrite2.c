#define _GNU_SOURCE
#include <sys/uio.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct region_info {
    void *addr;
    size_t len;
};

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(pipefd[0]);
        size_t pagesz = sysconf(_SC_PAGESIZE);
        char *mem = mmap(NULL, pagesz * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,-1, 0);

        if (mem == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        /* Fill first page with pattern */
        memset(mem, 'A', pagesz);

        /* Protect second page completely */
        if (mprotect(mem + pagesz, pagesz, PROT_NONE) < 0) {
            perror("mprotect");
            exit(1);
        }

        struct region_info info;
        info.addr = mem + pagesz ;   // Start near end of page 1
        info.len  = 128;                 // Cross into protected page
        write(pipefd[1], &info, sizeof(info));

        printf("Child PID: %d\n", getpid());
        printf("Page size: %zu\n", pagesz);
        printf("Read start address: %p\n", info.addr);
        printf("Transfer length: %zu bytes\n", info.len);
        printf("Second page is PROT_NONE\n");

        fflush(stdout);
        sleep(10);
        munmap(mem, pagesz * 2);
        exit(0);
    }

    close(pipefd[1]);
    struct region_info info;
    ssize_t r = read(pipefd[0], &info, sizeof(info));
    if (r != sizeof(info)) {
        perror("read");
        return 1;
    }

    printf("\nParent attempting cross-page read...\n");
    char buf[256] = {0};

    struct iovec local = {
        .iov_base = buf,
        .iov_len  = info.len
    };
    struct iovec remote = {
        .iov_base = info.addr,
        .iov_len  = info.len
    };

    errno = 0;
    ssize_t nread = process_vm_readv( pid,&local, 1,&remote, 1,0 );
    if (nread < 0) {
        perror("process_vm_readv");
    } else {
        printf("process_vm_readv read %zd bytes\n", nread);
        printf("First few bytes read: %.16s\n", buf);
    }

    printf("errno = %d (%s)\n", errno, strerror(errno));
    close(pipefd[0]);
    wait(NULL);
    return 0;
}
