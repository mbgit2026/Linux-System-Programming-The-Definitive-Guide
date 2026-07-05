#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    mqd_t mq;
    void *buffer;
    unsigned int priority;
    struct mq_attr attr;

    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <mq-name>", argv[0]);
        exit(EXIT_FAILURE);
    }

    mq = mq_open(argv[1], O_RDONLY, 0644, &attr);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_getattr(mq, &attr) == -1) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
        } 
 
    if (mq_receive(mq, buffer, attr.mq_msgsize, &priority) == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Received message: %s ", buffer);
    printf("Priority: %u\n", priority);

    mq_close(mq);
    free(buffer);
    return 0;
}