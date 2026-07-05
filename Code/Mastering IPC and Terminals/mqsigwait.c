#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define QUEUE_NAME "/myqueue"

int main() {
    mqd_t mqdes;
    struct sigevent sev;
    struct mq_attr attr;
    sigset_t mask;
    int sig;

    /* Block SIGUSR1 so it can be handled via sigwait */
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        exit(1);
    }

    /* Open queue */
    mqdes = mq_open(QUEUE_NAME, O_RDONLY);
    if (mqdes == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    /* Register notification */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;

    if (mq_notify(mqdes, &sev) == -1) {
        perror("mq_notify");
        exit(1);
    }

    printf("Waiting for message...\n");

    /* Wait for signal synchronously */
    if (sigwait(&mask, &sig) != 0) {
        perror("sigwait");
        exit(1);
    }

    printf("Signal received: message available\n");

    /* Now it's safe to use normal functions */
    if (mq_getattr(mqdes, &attr) == -1) {
        perror("mq_getattr");
        exit(1);
    }

    char *buf = malloc(attr.mq_msgsize);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }

    ssize_t nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
    if (nr == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Received message (%zd bytes): %s\n", nr, buf);

    free(buf);
    mq_close(mqdes);
    return 0;
}