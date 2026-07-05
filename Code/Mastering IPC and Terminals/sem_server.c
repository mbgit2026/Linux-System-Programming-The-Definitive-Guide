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

    printf("Server: waiting for request...\n");
    sem_wait(req);  // ⏳ wait for client

    printf("Server: processing request...\n");
    sleep(2);

    printf("Server: sending response...\n");
    sem_post(res);  // 🔔 respond

    sem_close(req);
    sem_close(res);

    /* cleanup (only once, typically server) */
    sem_unlink(REQ_SEM);
    sem_unlink(RES_SEM);

    return 0;
}