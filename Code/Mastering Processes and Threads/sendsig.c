#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t usr_interrupt = 0;

void synch_signal (int sig)
{
    usr_interrupt = 1;
}

void child_function (void)
{

    printf ("CHILD: pid is %d.\n", (int) getpid ());
    kill (getppid (), SIGUSR1);
    puts ("CHILD: exiting..");
    exit (0);
}

int main (void)
{
    struct sigaction usr_action;
    sigset_t block_mask;
    pid_t child_id;

    /* Establish the signal handler. */
    sigfillset (&block_mask);
    usr_action.sa_handler = synch_signal;
    usr_action.sa_mask = block_mask;
    usr_action.sa_flags = 0;
    sigaction (SIGUSR1, &usr_action, NULL);

    /* Create the child process. */
    child_id = fork ();
    if (child_id == 0)
        child_function ();
    
  
    while (!usr_interrupt)
    ;
    puts ("Finish!");
    return 0;
}