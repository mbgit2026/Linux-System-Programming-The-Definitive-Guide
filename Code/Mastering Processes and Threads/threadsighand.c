#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static volatile int running = 1;

/* Worker thread */

void *worker(void *arg)
{
    while (running) {
        printf("Worker thread doing work...\n");
        sleep(1);
    }

    printf("Worker thread shutting down\n");
    return NULL;
}

/* Signal handling thread */

void *signal_thread(void *arg)
{
    sigset_t *set = arg;
    int sig, s;

    for (;;) {

        s = sigwait(set, &sig);
        if (s != 0)
            errx(EXIT_FAILURE, "sigwait: %s", strerror(s));

        switch (sig) {

            case SIGUSR1:
                printf("Signal thread: SIGUSR1 received (reload config)\n");
                break;

            case SIGINT:
            case SIGTERM:
                printf("Signal thread: shutdown signal received\n");
                running = 0;
                return NULL;
        }
    }
}

int main(void)
{
    pthread_t sig_tid, worker_tid;
    sigset_t set;
    int s;

    /* Block signals in all threads */

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGUSR1);

    s = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_sigmask: %s", strerror(s));

    /* Create signal thread */

    s = pthread_create(&sig_tid, NULL, signal_thread, &set);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create");

    /* Create worker thread */

    s = pthread_create(&worker_tid, NULL, worker, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create");

    pthread_join(worker_tid, NULL);

    printf("Main exiting\n");
    return 0;
}