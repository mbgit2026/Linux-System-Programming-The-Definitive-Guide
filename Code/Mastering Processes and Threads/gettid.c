#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    printf("Initial PID: %ld\n", (long)getpid());
    printf("Parent PPID: %ld\n", (long)getppid());
    fflush(stdout);
    

    pid_t pid = fork();

    if (pid > 0) {
        printf("Parent: PID=%ld TID=%ld\n", (long)getpid(), (long)gettid());
        sleep(2);
    } else if (pid == 0) {
        printf("Child:  PID=%ld TID=%ld\n", (long)getpid(), (long)gettid());
        printf("Parent PPID: %ld\n", (long)getppid());
    } else {
        perror("fork");
    }

    return 0;
}
