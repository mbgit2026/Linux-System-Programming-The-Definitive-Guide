#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>

int main() {
    const char *name = "/my_semaphore";

    // Open existing semaphore
    sem_t *sem = sem_open(name, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    printf("Consumer: waiting for signal...\n");
    sem_wait(sem);  // blocks until producer calls sem_post

    printf("Consumer: received signal!\n");

    sem_close(sem);

    // Optional cleanup (only one process should do this)
    sem_unlink(name);

    return 0;
}