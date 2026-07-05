#include <unistd.h>
#include <stdio.h>

int main(void)
{
    char *argv[] = { "/bin/ls", "-l", "/", NULL };
    char *envp[] = { NULL };

    execve("/bin/ls", argv, envp);
    perror("execve");
    return 1;
}
