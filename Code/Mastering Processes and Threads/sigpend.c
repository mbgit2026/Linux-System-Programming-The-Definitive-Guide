#include <signal.h>
#include <stddef.h>
#include <stdio.h>

int main() {

    sigset_t base_mask, waiting_mask;

    sigemptyset(&base_mask);
    sigaddset(&base_mask, SIGINT);
    sigaddset(&base_mask, SIGTSTP);

    /* Block user interrupts while doing other processing. */
    sigprocmask(SIG_SETMASK, &base_mask, NULL);

    printf("SIGINT and SIGTSTP are blocked.\n");
    printf("Try pressing Ctrl-C or Ctrl-Z, then press Enter...\n");

    getchar();

    /* After a while, check to see whether any signals are pending. */
    sigpending(&waiting_mask);

    if (sigismember(&waiting_mask, SIGINT)) {
        printf("User has tried to interrupt the process (SIGINT)\n");
    }
    else if (sigismember(&waiting_mask, SIGTSTP)) {
        printf("User has tried to stop the process (SIGTSTP)\n");
    }
    else {
        printf("No pending signals.\n");
    }

    return 0;
}