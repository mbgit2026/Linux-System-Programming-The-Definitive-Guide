#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define QUEUE_NAME "/myqueue"

mqd_t mqdes;

void handler(int sig) {
    struct mq_attr attr;
    char *buf;
    ssize_t nr;

    printf("Signal received: message available\n");

    if (mq_getattr(mqdes, &attr) == -1) {
        perror("mq_getattr");
        exit(1);
    }

    buf = malloc(attr.mq_msgsize);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }

    nr = mq_receive(mqdes, buf, attr.mq_msgsize, NULL);
    if (nr == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Received message (%zd bytes): %s\n", nr, buf);

    free(buf);
    exit(0);  // terminate after one message
}

int main() {
    struct sigevent sev;
    struct sigaction sa;

    /* Open existing queue */
    mqdes = mq_open(QUEUE_NAME, O_RDONLY);
    if (mqdes == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    /* Install signal handler */
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    /* Set notification via signal */
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGUSR1;

    if (mq_notify(mqdes, &sev) == -1) {
        perror("mq_notify");
        exit(1);
    }

    printf("Waiting for message...\n");

    pause();  // wait for signal

    return 0;
}