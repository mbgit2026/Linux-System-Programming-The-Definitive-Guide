#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int sig)
{
    void *sp;
    asm("mov %%rsp, %0" : "=r"(sp));

    char buf[128];
    int len = snprintf(buf, sizeof(buf), "SIGSEGV caught\ncurrent stack pointer: %p\n",sp);

    write(STDOUT_FILENO, buf, len);
    _exit(EXIT_SUCCESS);
}

void overflow_stack(void)
{
    char buffer[1024];
    (void)buffer;
    overflow_stack();   /* infinite recursion */
}

int main(void)
{
    stack_t ss;
    struct sigaction sa;

    /* Establish alternate signal stack */
    ss.ss_sp = malloc(SIGSTKSZ);
    if (ss.ss_sp == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;

    if (sigaltstack(&ss, NULL) == -1) {
        perror("sigaltstack");
        exit(EXIT_FAILURE);
    }

    /* Install SIGSEGV handler that runs on the alternate stack */
    sa.sa_flags = SA_ONSTACK;
  //  sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Triggering stack overflow...\n");
    overflow_stack();
    return 0;
}