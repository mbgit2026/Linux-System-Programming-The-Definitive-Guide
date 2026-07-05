#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

#define REQ_SEM "/req_sem"
#define RES_SEM "/res_sem"

int main() {
    sem_t *req = sem_open(REQ_SEM, O_CREAT, 0644, 0);
    sem_t *res = sem_open(RES_SEM, O_CREAT, 0644, 0);

    if (req == SEM_FAILED || res == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Client: sending request...\n");
    sem_post(req);  // 🔔 send request

    printf("Client: waiting for response...\n");
    sem_wait(res);  // ⏳ wait for response

    printf("Client: received response!\n");

    sem_close(req);
    sem_close(res);

    return 0;
}