#define _GNU_SOURCE
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <unistd.h>
#include <stdint.h>

#define STACK_SIZE (1024 * 1024)
#define TLS_SIZE   (1024 * 8)

static int shared_counter = 0;
static int child_tid = 0;

/* Futex wait helper */
static void futex_wait(int *addr)
{
    syscall(SYS_futex, addr, FUTEX_WAIT, 1, NULL, NULL, 0);
}

static int thread_func(void *arg)
{
    shared_counter++;
    printf("Thread: shared_counter=%d\n", shared_counter);
    return 0;
}

int main(void)
{
    char *stack;
    void *tls;
    pid_t tid;

    stack = mmap(NULL, STACK_SIZE,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK,
                 -1, 0);
    tls = mmap(NULL, TLS_SIZE,
               PROT_READ | PROT_WRITE,
               MAP_PRIVATE | MAP_ANONYMOUS,
               -1, 0);

    if (stack == MAP_FAILED || tls == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    tid = clone(thread_func,
                stack,
                CLONE_VM |
                CLONE_FS |
                CLONE_FILES |
                CLONE_SIGHAND |
                CLONE_THREAD |
                CLONE_SETTLS |
                CLONE_CHILD_CLEARTID,
                NULL,
                NULL,
                tls,
                &child_tid);

    if (tid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    /* Join: wait until kernel clears child_tid */
    while (child_tid != 0)
        futex_wait(&child_tid);

    printf("Parent: shared_counter=%d\n", shared_counter);

    return 0;
}

