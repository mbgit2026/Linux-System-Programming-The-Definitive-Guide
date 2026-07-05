#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *argv[] = { "./hello.sh", NULL };

    char *envp[] = {
        "FOO=custom_env",
        "PATH=/usr/bin:/bin",
        NULL
    };

    execve("./hello.sh", argv, envp);
    perror("execve");
    return 1;
}
