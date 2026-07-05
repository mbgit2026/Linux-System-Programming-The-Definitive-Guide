#include <stdio.h>
#include <sys/msg.h>
#include <stdlib.h>
#include "message.h"


int main() {
    key_t key = ftok("progfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct msgbuf message;

    msgrcv(msgid, &message, sizeof(message.mtext), 1, 0);

    printf("Received message: %s\n", message.mtext);

    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}