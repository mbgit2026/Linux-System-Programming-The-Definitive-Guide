#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(void)
{
    char *argv[] = { "/usr/bin/env", NULL };

    int count = 0;
    while (environ[count]) count++;

    char **envp = malloc((count + 2) * sizeof(char *));
    memcpy(envp, environ, count * sizeof(char *));
    envp[count] = "FOO=custom";
    envp[count + 1] = NULL;

    execve("/usr/bin/env", argv, envp);
}
