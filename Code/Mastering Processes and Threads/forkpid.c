#include <stdio.h>
#include <unistd.h>

int main(void)
{
    printf("PID: %ld\n", (long)getpid());
    printf("PPID: %ld\n\n", (long)getppid());
    fflush(stdout);
    

    pid_t pid = fork();

    if (pid > 0) {
        printf("Parent: PID=%ld\n", (long)getpid());
        sleep(2);
    } else if (pid == 0) {
        printf("Child:  PID=%ld\n", (long)getpid());
        printf("Child PPID: %ld\n", (long)getppid());
    } else {
        perror("fork");
    }

    return 0;
}