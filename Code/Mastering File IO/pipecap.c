#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define CHUNK_SIZE   4096
#define TOTAL_WRITES 10000

static long now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int main(void)
{
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    /* Show original pipe size */
    int orig = fcntl(pipefd[0], F_GETPIPE_SZ);
    printf("Original pipe size: %d bytes\n", orig);

    /* Try to enlarge the pipe */
    int requested = orig * 4;
    int actual = fcntl(pipefd[0], F_SETPIPE_SZ, requested);
    if (actual == -1) {
        perror("F_SETPIPE_SZ");
        actual = orig;
    }

    printf("Pipe size in use: %d bytes\n\n", actual);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ---------------- Consumer ---------------- */
        close(pipefd[1]);

        char buf[CHUNK_SIZE];
        ssize_t n;

        while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
            /* Simulate slow processing */
            usleep(200);
        }

        close(pipefd[0]);
        _exit(0);
    }

    /* ---------------- Producer ---------------- */
    close(pipefd[0]);

    /* Make pipe non-blocking so we can observe blocking behavior */
    int flags = fcntl(pipefd[1], F_GETFL);
    fcntl(pipefd[1], F_SETFL, flags | O_NONBLOCK);

    char buf[CHUNK_SIZE];
    memset(buf, 'A', sizeof(buf));

    int would_block = 0;
    long start = now_ns();

    for (int i = 0; i < TOTAL_WRITES; i++) {
        ssize_t n = write(pipefd[1], buf, sizeof(buf));
        if (n == -1) {
            if (errno == EAGAIN) {
                would_block++;
                /* Back off briefly and retry */
                usleep(50);
                i--;
            } else {
                perror("write");
                break;
            }
        }
    }

    long end = now_ns();

    close(pipefd[1]);
    wait(NULL);

    printf("Producer statistics:\n");
    printf("  write attempts: %d\n", TOTAL_WRITES);
    printf("  would-block events: %d\n", would_block);
    printf("  elapsed time: %.3f ms\n",
           (end - start) / 1e6);

    return 0;
}
