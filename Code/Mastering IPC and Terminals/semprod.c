#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <unistd.h>

int main() {
    const char *name = "/my_semaphore";

    // Create semaphore with initial value 0
    sem_t *sem = sem_open(name, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Producer: doing some work...\n");
    sleep(3);

    printf("Producer: signaling semaphore\n");
    sem_post(sem);  // signal consumer

    sem_close(sem);

    return 0;
}