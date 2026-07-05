#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/wait.h>

void crash() {
    printf("Child dumpable = %d\n", prctl(PR_GET_DUMPABLE));    
    printf("Child: about to crash...\n");
    fflush(stdout);
    raise(SIGSEGV);
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Usage: %s [dump|nodump]\n", argv[0]);
        return 1;
    }

    int dumpable = strcmp(argv[1], "nodump") == 0 ? 0 : strcmp(argv[1], "dump") == 0 ? 1 : -1;
    if (dumpable == -1) {
        printf("Invalid argument. Use dump or nodump\n");
        return 1;
    }

     if (prctl(PR_SET_NAME, "crashable_prog", 0,0,0) == -1) {
        perror("prctl");
        exit(1);
    }
  
    if (prctl(PR_SET_DUMPABLE, dumpable, 0, 0, 0) == -1) {
        perror("prctl(PR_SET_DUMPABLE)");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        printf("Child PID %d running\n", getpid());
        crash();
        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status)) {
            printf("Child killed by signal %d\n", WTERMSIG(status));
#ifdef WCOREDUMP
    if (WCOREDUMP(status))
        printf(" (core dumped)");
    else
        printf(" (no core dump)");
#endif
        printf("\n");
        } else if (WIFEXITED(status)) 
            printf("Child exited normally (%d)\n", WEXITSTATUS(status));
    }

    getchar();
    return 0;
}
