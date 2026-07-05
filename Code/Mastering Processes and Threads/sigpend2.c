#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static void sig_quit(int signo)
{
    printf("caught SIGQUIT\n");
    if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
        perror("signal");
}
int main(void)
{
    sigset_t newmask, oldmask, pendmask;
  
    if (signal(SIGQUIT, sig_quit) == SIG_ERR)
        perror("signal");
    
     /* Block SIGQUIT and save current signal mask. */
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        perror("sigprocmask");

    sleep(5);
    
    /* SIGQUIT here will remain pending */
    if (sigpending(&pendmask) < 0)
        perror("sigpending");
    
    if (sigismember(&pendmask, SIGQUIT))
        printf("\nSIGQUIT pending\n");
    
    /* Restore signal mask which unblocks SIGQUIT. */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        perror("sigprocmask");
    
    printf("SIGQUIT unblocked\n");
    sleep(5);
    exit(0);
    /* SIGQUIT here will terminate with core file */
}


