#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 2
#define NUM_INCREMENTS 100000

int counter = 0;
sem_t mutex;

void* worker(void* arg) {
    for (int i = 0; i < NUM_INCREMENTS; i++) {
        sem_wait(&mutex);   // lock

        counter++;          // critical section

        sem_post(&mutex);   // unlock
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    // Initialize semaphore as binary (1 = unlocked)
    if (sem_init(&mutex, 0, 1) == -1) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, worker, NULL) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", counter);

    sem_destroy(&mutex);
    return 0;
}