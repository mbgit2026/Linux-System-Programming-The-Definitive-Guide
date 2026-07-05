#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>

#define QUEUE_NAME "/myqueue"
#define MAX_SIZE 128

int main() {
    mqd_t mq;
    struct mq_attr attr;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, NULL);
    if (mq == (mqd_t) -1) {
        perror("mq_open");
        exit(1);
    }

    char message[MAX_SIZE];
    printf("Enter message: ");
    fgets(message, MAX_SIZE, stdin);
    if (mq_send(mq, message, strlen(message) + 1, 1) == -1) {
        perror("mq_send");
        exit(1);
    }

    printf("Message sent: %s\n", message);
    mq_close(mq);
    return 0;
}