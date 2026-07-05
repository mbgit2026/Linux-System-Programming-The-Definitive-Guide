#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/acct.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#define ACCT_FILE "/var/account/pacct"

int main()
{
    pid_t pid1, pid2;

    /* Must run as root to enable accounting + trigger ASU */
    if (geteuid() != 0) {
        fprintf(stderr, "Run as root\n");
        return 1;
    }

    /* Enable accounting */
    if (acct(ACCT_FILE) == -1) {
        perror("acct");
        return 1;
    }

    printf("Accounting enabled\n");


    /* ---------- Child 1 : AFORK ---------- */
    pid1 = fork();

    if (pid1 < 0) {
        perror("fork");
        return 1;
    }

    if (pid1 == 0) {
        /* Child 1: fork but DO NOT exec -> should trigger AFORK */
        printf("Child 1 exiting without exec\n");
        _exit(0);
    }

    /* ---------- Child 2 : ASU + exec ---------- */
    pid2 = fork();

    if (pid2 < 0) {
        perror("fork");
        return 1;
    }

    if (pid2 == 0) {
        printf("Child 2 running dd via execl\n");

        execl("/bin/dd",
              "dd",
              "if=/etc/passwd",
              "of=/dev/null",
              NULL);

        perror("execl failed");
        _exit(1);
    }

    /* Parent waits */
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("Children finished\n");

     /* Disable accounting */
    if (acct(NULL) == -1) {
        perror("acct disable");
        return 1;
    }

    printf("Accounting disabled\n");

    return 0;
}