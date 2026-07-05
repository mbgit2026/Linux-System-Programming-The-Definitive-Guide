#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <err.h>

#define MAX_SIZE 128

int main(int argc, char *argv[]) {
    mqd_t mq;
    struct mq_attr attr;
    char *message;

     if (argc != 4) {
        fprintf(stderr, "Usage: %s <mq-name> <msg> <pri> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    message = argv[2];
    printf("%s %d", message, strlen(message));

    mq = mq_open(argv[1], O_CREAT | O_WRONLY, 0644, NULL);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(1);
    }

    if (mq_send(mq, message, strlen(message) + 1, atoi(argv[3])) == -1) {
        perror("mq_send");
        exit(1);
    }

    mq_close(mq);
    return 0;
}