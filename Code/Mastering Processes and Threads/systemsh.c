#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    int status;

status = system("ls /root");
    if (status == -1) {
        perror("system");
        return 1;
    }

    if (WIFEXITED(status)) {
        printf("Command exited normally\n");
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Command killed by signal %d\n", WTERMSIG(status));

    }

    return 0;
}
