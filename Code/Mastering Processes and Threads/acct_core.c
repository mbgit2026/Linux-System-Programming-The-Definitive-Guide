#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/acct.h>
#include <sys/wait.h>
#include <signal.h>

#define ACCT_FILE "/var/account/pacct"

int main()
{
    pid_t pid;

    if (geteuid() != 0) {
        printf("Run as root\n");
        return 1;
    }

    if (acct(ACCT_FILE) == -1) {
        perror("acct enable");
        return 1;
    }

    printf("Accounting enabled\n");

    pid = fork();

    if (pid == 0) {
        printf("Child 2 causing segmentation fault (core dump)\n");
        int *p = NULL;
        *p = 42;
        _exit(1);
    }

    waitpid(pid, NULL, 0);

    printf("Child finished\n");

    if (acct(NULL) == -1) {
        perror("acct disable");
        return 1;
    }

    printf("Accounting disabled\n");

    return 0;
}