#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void *signal_sender(void *arg) {
    pthread_t target = *(pthread_t *)arg;

    sleep(2);
    pthread_kill(target, SIGUSR1);

    sleep(2);
    pthread_kill(target, SIGQUIT);

    return NULL;
}

/* Simple error handling functions */

static void *sig_thread(void *arg)
{
    sigset_t *set = arg;
    int s, sig;

    for (;;) {
        s = sigwait(set, &sig);
        if (s != 0)
            errx(EXIT_FAILURE, "sigwait: %s", strerror(s));
        printf("Signal handling thread got signal %d\n", sig);
    }
}

int main(void)
{
    pthread_t thread;
    pthread_t sender;
    sigset_t set;
    int s;

    /* Block SIGQUIT and SIGUSR1; other threads created by main()
        will inherit a copy of the signal mask.  */

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_sigmask: %s", strerror(s));

    s = pthread_create(&thread, NULL, &sig_thread, &set);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create: %s", strerror(s));
        
    s = pthread_create(&sender, NULL, signal_sender, &thread);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create: %s", strerror(s));

    pause();            /* Dummy pause so we can test program */


}

