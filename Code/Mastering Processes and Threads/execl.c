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
        exit(1);

    } else if (pid == 0) { /* Child 1: explicit path + custom environment */
        if (execle("./echoall", "echoall", "myarg1", "MY ARG2", (char *)0, env_init) < 0) {
     //   if (execle("/usr/bin/echo", "echo", "myarg1", "MY ARG2", (char *)0, env_init) < 0) {
            perror("execle");
            _exit(127);   /* Important: do not return to parent code */
        }
    }

    if (waitpid(pid, NULL, 0) < 0)
        perror("wait");

    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);

    } else if (pid == 0) { /* Child 2: PATH search + inherited environment */

        if (execlp("echoall", "echoall", "only 1 arg", (char *)0) < 0) {
       // if (execlp("echo", "echo", "only 1 arg", (char *)0) < 0) {
            perror("execlp");
            _exit(127);
        }
    }

    if (waitpid(pid, NULL, 0) < 0)
        perror("wait");

    return 0;
}
