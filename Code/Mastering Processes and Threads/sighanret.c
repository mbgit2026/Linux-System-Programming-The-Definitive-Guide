#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t keep_going = 1;

void catch_alarm (int sig)
{
    keep_going = 0;
    //signal (sig, catch_alarm);
}
void do_stuff (void)
{
    puts ("Waiting for alarm....");
}

int main (void)
{
    signal (SIGALRM, catch_alarm);

    alarm (2);

    while (keep_going)
        do_stuff ();

    return EXIT_SUCCESS;
}