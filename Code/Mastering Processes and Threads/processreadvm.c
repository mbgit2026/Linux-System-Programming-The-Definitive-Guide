#define _GNU_SOURCE
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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

        char secret[] = "Hello from child process!";
        void *addr = secret;
        size_t len = sizeof(secret);

        write(pipefd[1], &addr, sizeof(addr));
        write(pipefd[1], &len, sizeof(len));

        printf("Child PID: %d\n", getpid());
        printf("Child buffer address: %p\n", addr);
        printf("Child buffer content: %s\n", secret);

        fflush(stdout);
        sleep(10); 

        close(pipefd[1]);
        return 0;
    }

    close(pipefd[1]);

    void *remote_addr;
    size_t size;

    read(pipefd[0], &remote_addr, sizeof(remote_addr));
    read(pipefd[0], &size, sizeof(size));

    printf("\nParent reading from child PID %d\n", pid);
    printf("Remote address: %p\n", remote_addr);
    printf("Size: %zu\n", size);

    char *local_buf = malloc(size);
    if (!local_buf) {
        perror("malloc");
        return 1;
    }

    struct iovec local_iov = {
        .iov_base = local_buf,
        .iov_len  = size
    };

    struct iovec remote_iov = {
        .iov_base = remote_addr,
        .iov_len  = size
    };

    ssize_t nread = process_vm_readv(pid, &local_iov, 1, &remote_iov, 1, 0);

    if (nread < 0) {
        perror("process_vm_readv");
        return 1;
    }

    printf("\nParent read %zd bytes\n", nread);
    printf("Parent sees: %s\n", local_buf);

    free(local_buf);
    close(pipefd[0]);

    wait(NULL);
    return 0;
}

