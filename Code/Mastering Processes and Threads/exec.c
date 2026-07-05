#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


char *env_init[] = { "USER=unknown", "PATH=/tmp", NULL };

int main(void)
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) { /* specify pathname, specify environment */
        if (execle("/usr/bin/echo", "echo", "myarg1", "MY ARG2", (char *)0, env_init) < 0)
            perror("execle");
    }
    if (waitpid(pid, NULL, 0) < 0)
        perror("wait");
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) { /* specify filename, inherit environment */
        if (execlp("echo", "echo", "only 1 arg", (char *)0) < 0)
            perror("execlp");
    }
    exit(0);
}