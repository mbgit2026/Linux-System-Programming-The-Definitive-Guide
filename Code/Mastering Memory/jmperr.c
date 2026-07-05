#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

sigjmp_buf env;

/* -------- Deeply nested calls -------- */

void level3(void)
{
    printf("level3: fatal error occurred\n");
    siglongjmp(env, 1);   // deep error
}

void level2(void)
{
    printf("level2\n");
    sleep(1);             // slow down execution
    level3();
}

void level1(void)
{
    printf("level1\n");
    sleep(1);
    level2();
}

/* -------- Signal handler -------- */

void handler(int sig)
{
    const char msg[] = "\nSignal handler: SIGINT caught\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    siglongjmp(env, 2);   // signal jump
}

int main(void)
{
    int ret;

    /* Save context and signal mask */
    ret = sigsetjmp(env, 1);

    if (ret == 0) {
        /* Initial execution */
        printf("Main: program started\n");
        printf("Press Ctrl-C now, or wait for the error...\n");

        struct sigaction sa = {0};
        sa.sa_handler = handler;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGINT, &sa, NULL);

        /* Wait a bit before triggering deep error */
        sleep(3);

        /* Trigger nested calls (deep error) */
        level1();

        printf("This line is never reached\n");

    } else if (ret == 1) {
        /* Returned from deep error */
        printf("Main: recovered from deep error\n");

    } else if (ret == 2) {
        /* Returned from signal handler */
        printf("Main: recovered from SIGINT\n");
    }

    printf("Main: continuing execution safely\n");
    return 0;
}

