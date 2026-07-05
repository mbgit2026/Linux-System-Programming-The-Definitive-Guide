#include <stdio.h>
#include <sys/msg.h>
#include "message.h"
#include <stdlib.h>


int main() {
    key_t key = ftok("progfile", 65);
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct msgbuf message;
    message.mtype = 1;

    printf("Enter message: ");
    fgets(message.mtext, sizeof(message.mtext), stdin);

    msgsnd(msgid, &message, sizeof(message.mtext), 0);

    printf("Message sent: %s\n", message.mtext);

    return 0;
}