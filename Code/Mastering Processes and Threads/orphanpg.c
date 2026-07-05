#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <termios.h>

void print_ids(const char *name)
{
    printf("%s: pid = %ld, ppid = %ld, pgrp = %ld, tpgrp = %ld\n",
           name,
           (long)getpid(),
           (long)getppid(),
           (long)getpgrp(),
           (long)tcgetpgrp(STDIN_FILENO));
}

void handler(int sig)
{
    if (sig == SIGHUP)
        printf("Child: received SIGHUP\n");
    if (sig == SIGCONT)
        printf("Child: received SIGCONT\n");
}

int main(void)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid > 0) {
        /* Parent */
        print_ids("parent:");
        sleep(5);
        printf("Parent exiting...\n");
        exit(0);
    }

    /* Child */
    /* Put child in its own process group */
    if (setpgid(0, 0) < 0) {
        perror("setpgid");
        exit(1);
    }

    /* Install signal handlers */
    signal(SIGHUP, handler);
    signal(SIGCONT, handler);

    print_ids("child:");

    printf("Child stopping itself...\n");
    kill(getpid(), SIGSTOP);   /* Stop the child */

    /* After being resumed */
    print_ids("child:");

    printf("Child exiting\n");
    return 0;
}