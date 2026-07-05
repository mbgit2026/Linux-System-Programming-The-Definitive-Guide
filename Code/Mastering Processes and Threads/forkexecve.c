#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int main(void)
{
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char *argv[] = { "/bin/ls", "-l", "/", NULL };

        execve("/bin/ls", argv, environ);
        perror("execve");
        _exit(EXIT_FAILURE);
    }

    printf("Parent: waiting for child %d\n", pid);
    wait(NULL);
    printf("Parent: child finished\n");
    return 0;
}
