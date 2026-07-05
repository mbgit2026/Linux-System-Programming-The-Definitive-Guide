#define _GNU_SOURCE
#include <sys/uio.h>
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
        static char buffer[128] = "Original message from child";
        struct region_info info = {
            .addr = buffer,
            .len  = sizeof(buffer)
        };

        write(pipefd[1], &info, sizeof(info));
        printf("Child PID: %d\n", getpid());
        printf("Child buffer addr: %p\n", buffer);
        printf("Child initial content: %s\n", buffer);
        fflush(stdout);

        sleep(5); 
        printf("\nChild after parent write: %s\n", buffer);
        close(pipefd[1]);
        return 0;
    }

    close(pipefd[1]);
    struct region_info info;
    read(pipefd[0], &info, sizeof(info));

    printf("\nParent working on child PID %d\n", pid);
    char read_buf[128] = {0};

    struct iovec local_iov = {
        .iov_base = read_buf,
        .iov_len  = info.len
    };

    struct iovec remote_iov = {
        .iov_base = info.addr,
        .iov_len  = info.len
    };

    ssize_t nread = process_vm_readv(pid,&local_iov, 1,&remote_iov, 1,0);
    if (nread < 0) {
        perror("process_vm_readv");
        return 1;
    }

    printf("\nParent read (%zd bytes): %s\n", nread, read_buf);
    char write_buf[128] = "Modified by parent using process_vm_writev()";
   
    local_iov.iov_base = write_buf;
    local_iov.iov_len  = strlen(write_buf) + 1;
    remote_iov.iov_base = info.addr;
    remote_iov.iov_len  = strlen(write_buf) + 1;

    ssize_t nwritten = process_vm_writev(pid, &local_iov, 1, &remote_iov, 1, 0);
    if (nwritten < 0) {
        perror("process_vm_writev");
        return 1;
    }

    printf("Parent wrote %zd bytes to child\n", nwritten);
    close(pipefd[0]);
    wait(NULL);
    return 0;
}
