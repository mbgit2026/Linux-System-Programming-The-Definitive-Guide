#define _GNU_SOURCE
#include <linux/kcmp.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK_SIZE (1024 * 1024)

static int child_func(void *arg)
{
    sleep(1);
    return 0;
}

int main(int argc, char *argv[])
{
    pid_t pid;
    void *stack = NULL;

    printf("Parent PID: %d\n", getpid());

    if (argc > 1) {
        /* --- Use clone() --- */
        printf("Creating child with clone(CLONE_VM)\n");

        stack = malloc(STACK_SIZE);
        if (!stack) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        pid = clone(child_func,
                    stack + STACK_SIZE,
                    CLONE_VM | SIGCHLD,
                    NULL);
        if (pid == -1) {
            perror("clone");
            exit(EXIT_FAILURE);
        }
    } else {
        /* --- Use fork() --- */
        printf("Creating child with fork()\n");

        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            sleep(1);
            _exit(0);
        }
    }

    /* Parent: compare address spaces */
    int ret = syscall(SYS_kcmp, getpid(), pid, KCMP_VM, 0, 0);

    printf("KCMP_VM comparison: %s\n",
           ret == 0 ? "same address space" : "different address spaces");

    waitpid(pid, NULL, 0);

    free(stack);
    return 0;
}
