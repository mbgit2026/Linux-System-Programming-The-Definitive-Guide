#include <signal.h>
#include <stdio.h>
#include <errno.h>

void pr_mask(const char *str)
{

    sigset_t sigset;
    int errno_save;
    errno_save = errno;
    
    /* we can be called by signal handlers */
    if (sigprocmask(0, NULL, &sigset) < 0) {
        perror("sigprocmask");
    } else {
        printf("%s", str);
        if (sigismember(&sigset, SIGINT))
            printf(" SIGINT");
        if (sigismember(&sigset, SIGQUIT))
            printf(" SIGQUIT");
        if (sigismember(&sigset, SIGUSR1))
            printf(" SIGUSR1");
        if (sigismember(&sigset, SIGALRM))
            printf(" SIGALRM");
        printf("\n");
    }
    errno = errno_save;
}

/*
static void sig_int(int signo)
{
    pr_mask("\nin sig_int: ");
}
    */