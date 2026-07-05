#include <unistd.h>
#include <stdio.h>

int main(void)
{
    char *argv[] = {
        "/usr/bin/env",
        NULL
    };

    char *envp[] = {
        "FOO=hello",
        "BAR=42",
        "PATH=/usr/bin:/bin",
        NULL
    };

    execve("/usr/bin/env", argv, envp);

    perror("execve");
    return 1;
}
