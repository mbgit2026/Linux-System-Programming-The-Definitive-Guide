#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

void *sig_thread(void *arg)
{
    sigset_t *set = arg;
    int sig;

    for (;;) {
        sigwait(set, &sig);
        printf("Received signal %d\n", sig);
    }
}

void *sig_thread2(void *arg)
{
    int sig;

    printf("Thread 2 executing...\n");
    sleep(10);
    return NULL;
}

int main(void)
{
    pthread_t tid, tid2;
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);

    /* Block signals in all threads */
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    pthread_create(&tid2, NULL, sig_thread2, NULL);
    /* Create signal-handling thread */
    pthread_create(&tid, NULL, sig_thread, &set);

    printf("PID: %d\n", getpid());
    pthread_join(tid, NULL);
}