#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int  globvar = 6;       /* external variable in initialized data */
char buf[] = "a write to stdout\n";

int main(void)
{
    int  var;
    pid_t pid;       /* automatic variable on the stack */
    var = 88;

    printf("%d", sysconf(_SC_CHILD_MAX));
    
    if (write(STDOUT_FILENO, buf, sizeof(buf)-1) != sizeof(buf)-1)
        perror("write");
    
    printf("before fork\n");
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid == 0) {      /* child */
        globvar++;              /* modify variables */
        var++;
    } else {
        sleep(2);               /* parent */
    }

    printf("pid = %ld, glob = %d, var = %d\n", (long)getpid(), globvar, var);
    exit(0);
}