#define _GNU_SOURCE
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define N_IOV 3

struct region_info {
    void *addr[N_IOV];
    size_t len[N_IOV];
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

        static char r1[] = "Region ONE data";
        static char r2[] = "Region TWO data";
        static char r3[] = "Region THREE data";

        struct region_info data;
        data.addr[0] = r1;
        data.len[0]  = sizeof(r1);
        data.addr[1] = r2;
        data.len[1]  = sizeof(r2);
        data.addr[2] = r3;
        data.len[2]  = sizeof(r3);

        write(pipefd[1], &data, sizeof(data));
        printf("Child PID: %d\n", getpid());
        printf("Child regions:\n");
        for (int i = 0; i < N_IOV; i++)
            printf("  [%d] %p (%zu bytes)\n", i, data.addr[i], data.len[i]);

        fflush(stdout);
        sleep(1);
        close(pipefd[1]);
        return 0;
    }

    close(pipefd[1]);
    struct region_info datarec;
    if (read(pipefd[0], &datarec, sizeof(datarec)) != sizeof(datarec)) {
        perror("read info");
        return 1;
    }

    printf("\nParent reading from PID %d using %d iovecs\n", pid, N_IOV);

    struct iovec remote_iov[N_IOV];
    struct iovec local_iov[N_IOV];
    char *local_bufs[N_IOV];

    for (int i = 0; i < N_IOV; i++) {
        local_bufs[i] = malloc(datarec.len[i]);
        if (!local_bufs[i]) {
            perror("malloc");
            return 1;
        }

        remote_iov[i].iov_base = datarec.addr[i];
        remote_iov[i].iov_len  = datarec.len[i];
        local_iov[i].iov_base = local_bufs[i];
        local_iov[i].iov_len  = datarec.len[i];
    }

    ssize_t nread = process_vm_readv(pid, local_iov, N_IOV, remote_iov, N_IOV, 0);
    if (nread < 0) {
        perror("process_vm_readv");
        return 1;
    }

    printf("\nTotal bytes read: %zd\n", nread);
    printf("\nParent received:\n");
    for (int i = 0; i < N_IOV; i++)
        printf("  [%d] %s\n", i, local_bufs[i]);

    for (int i = 0; i < N_IOV; i++)
        free(local_bufs[i]);

    close(pipefd[0]);
    wait(NULL);
    return 0;
}
