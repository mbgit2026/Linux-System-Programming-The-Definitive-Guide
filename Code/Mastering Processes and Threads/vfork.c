#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    pid_t pid;

    printf("Parent PID: %ld\n", (long)getpid());
    fflush(stdout);

    pid = vfork();

    if (pid == -1) {
        perror("vfork");
        _exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        char *argv[] = { "/bin/echo", "Hello from child", NULL };
        execve("/bin/echo", argv, NULL);

        perror("execve");
        _exit(EXIT_FAILURE);
    }
    printf("Parent resumed after child exec/exit\n");
    return 0;
}
