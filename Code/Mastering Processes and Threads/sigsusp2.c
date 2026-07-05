#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "prmask.h"


volatile sig_atomic_t quitflag;

static void sig_int(int signo) 
{
    if (signo == SIGINT)
        printf("\ninterrupt\n");
    else if (signo == SIGQUIT)
        quitflag = 1;
}

int main(void)
{
    sigset_t newmask, oldmask, zeromask;
    
    if (signal(SIGINT, sig_int) == SIG_ERR)
        perror("signal");
    
    if (signal(SIGQUIT, sig_int) == SIG_ERR)
        perror("signal");
    
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    
    /* Block SIGQUIT and save current signal mask. */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("sigprocmask");
    
    pr_mask("BEFORE sigsuspend: ");
    while (quitflag == 0)
        sigsuspend(&zeromask);

    pr_mask("AFTER sigsuspend: ");
    
    /* SIGQUIT has been caught and is now blocked; do whatever. */
    quitflag = 0;
    
    /* Reset signal mask which unblocks SIGQUIT. */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        perror("sigprocmask");

    pr_mask("AFTER sigprocmask: ");

    exit(0);
}