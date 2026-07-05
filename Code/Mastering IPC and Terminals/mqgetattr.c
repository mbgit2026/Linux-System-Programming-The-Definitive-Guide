#include <err.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    mqd_t mqd;
    struct mq_attr attr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    mqd = mq_open(argv[1], O_CREAT | O_EXCL, 0600, NULL);
    if (mqd == (mqd_t) -1)
        err(EXIT_FAILURE, "mq_open");

    if (mq_getattr(mqd, &attr) == -1)
        err(EXIT_FAILURE, "mq_getattr");

    printf("Maximum # of messages on queue:   %ld\n", attr.mq_maxmsg);
    printf("Maximum message size:             %ld\n", attr.mq_msgsize);

    if (mq_unlink(argv[1]) == -1)
        err(EXIT_FAILURE, "mq_unlink");

    exit(EXIT_SUCCESS);
}
