#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define STACK_SIZE (1024 * 1024)

int num;

int child_func(void *arg) {
    printf("From child: PID=%d\n", getpid());
    if (num) {
        write(*(int *) arg, "Hello from child\n", 17);
        if (close(*(int *) arg) <0) 
            perror("close");
        printf("From child: I closed the file \n");
    }
    fflush(stdout);
    return 0;
}

int main(int argc, char *argv[]) {
    void *stack;
    pid_t pid;
    int fd;

    if (argc > 1)
        num = atoi(argv[1]);

    fd = open("newfile.txt", O_RDWR | O_CREAT, 0644);
    write(fd, "Hello from parent\n", 18);

    stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pid = clone(child_func, stack + STACK_SIZE, SIGCHLD | CLONE_FILES, (void *)&fd);
    if (pid == -1) {
        perror("clone");
        exit(EXIT_FAILURE);
    }

   
    waitpid(pid, NULL, 0);
    if (write(fd, "Finish", 7) <0 ) 
        perror("write");
    
    if (close(fd) <0)
        perror("close");

    free(stack);
    return 0;
}