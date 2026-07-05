#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


volatile sig_atomic_t fatal_error_in_progress = 0;

void fatal_error_signal (int sig)
{
    if (fatal_error_in_progress)
        raise (sig);
    fatal_error_in_progress = 1;

    // cleanup code
    write(STDOUT_FILENO, "Fatal handler executed\n", 23);
    signal (sig, SIG_DFL);
    raise (sig);
}


int main (void)
{

signal(SIGSEGV, fatal_error_signal);

    printf("About to cause a segmentation fault...\n");
    sleep(1);

    raise(SIGSEGV);

    return EXIT_SUCCESS;
}