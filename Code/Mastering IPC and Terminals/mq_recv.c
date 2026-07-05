#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define QUEUE_NAME "/myqueue"
#define MAX_SIZE 128

int main() {
    mqd_t mq;
    char buffer[MAX_SIZE];
    unsigned int priority;
    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Open the queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(1);
    }

    // Receive message
    if (mq_receive(mq, buffer, MAX_SIZE, &priority) == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Received message: %s", buffer);
    printf("Priority: %u\n", priority);

    mq_close(mq);

    // Remove the queue
    //mq_unlink(QUEUE_NAME);

    return 0;
}