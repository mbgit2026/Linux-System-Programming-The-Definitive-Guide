#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/user.h>

int main() {
    pid_t child = fork();

    if (child == 0) {
        // Child process
        printf("Child: running\n");
        ptrace(PTRACE_TRACEME, 0, NULL, NULL); // Tell kernel to let parent trace me
        execl("/bin/ls", "ls", NULL);          // Replace child with /bin/ls
    } else {
        // Parent process
        int status;
        waitpid(child, &status, 0);            // Wait for child to stop

        printf("Parent: tracing child %d\n", child);

        while (WIFSTOPPED(status)) {
            struct user_regs_struct regs;
            // Get child registers
            ptrace(PTRACE_GETREGS, child, NULL, &regs);
            printf("Child RIP: %llx\n", regs.rip); // Print instruction pointer

            // Let child continue to next syscall
            ptrace(PTRACE_SYSCALL, child, NULL, NULL);
            waitpid(child, &status, 0);
        }

        printf("Parent: child finished\n");
    }

    return 0;
}