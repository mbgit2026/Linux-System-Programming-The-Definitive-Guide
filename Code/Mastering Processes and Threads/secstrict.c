#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <syscall.h>

int main() {

    printf("\nPID: %d\n", getpid());
    fflush(stdout);

    if (syscall(SYS_seccomp, SECCOMP_SET_MODE_STRICT, 0, NULL) == -1) {
        perror("syscall");
        exit (1);
    }

    printf("PID: %d", getpid());
    printf("No printed");

    exit(0);

}