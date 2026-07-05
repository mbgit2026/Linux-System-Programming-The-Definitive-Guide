#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Child exiting (PID=%ld)\n", (long)getpid());
        exit(EXIT_SUCCESS);
    } else {
        printf("Parent sleeping (PID=%ld), child PID=%ld\n",
               (long)getpid(), (long)pid);
       // sleep(30);   /* Do NOT call wait() */
        wait(NULL);
        printf("Parent exiting\n");
    }

    return 0;
}
