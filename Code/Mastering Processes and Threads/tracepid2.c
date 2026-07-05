#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t child = fork();

    if (child == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        execl("/bin/ls", "ls", NULL);
        perror("execl");
        exit(1);
    } else {
        int status;
        waitpid(child, &status, 0);

       ptrace(PTRACE_SYSCALL, child, NULL, NULL);

        while (1) {
            waitpid(child, &status, 0);

            if (WIFEXITED(status)) {
                printf("Child exited\n");
                break;
            }

            struct user_regs_struct regs;
            ptrace(PTRACE_GETREGS, child, NULL, &regs);

#ifdef __x86_64__
            long syscall = regs.orig_rax;
            printf("Syscall: %ld\n", syscall);
#else
            printf("Syscall tracing not implemented for this arch\n");
#endif

            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
    }

    return 0;
}