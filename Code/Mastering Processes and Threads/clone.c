#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)  

int child_func(void *arg) {
    printf("Hello from child! PID=%d\n", getpid());
    return 0;
}

int main(void) {
    void *stack;
    pid_t pid;

    stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pid = clone(child_func, stack + STACK_SIZE,CLONE_VM| SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

    printf("Hello from parent! Child PID=%d\n", pid);
    waitpid(pid, NULL, 0);
    free(stack);
    return 0;
}
