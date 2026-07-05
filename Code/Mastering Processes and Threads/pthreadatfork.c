#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Handlers
void prepare(void) {
    printf("prepare: locking mutex before fork\n");
    pthread_mutex_lock(&lock);
    printf("prepare: mutex locked\n");
}

void parent(void) {
    printf("parent: unlocking mutex after fork\n");
    pthread_mutex_unlock(&lock);
}

void child(void) {
    printf("child: unlocking mutex after fork\n");
    pthread_mutex_unlock(&lock);
}

void* thread_func(void* arg) {
    printf("Worker thread: attempting to lock mutex...\n");
    pthread_mutex_lock(&lock);
    printf("Worker thread: acquired mutex, sleeping...\n");
    sleep(5);
    printf("Worker thread: releasing mutex\n");
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t t;

    // Register fork handlers
    if (pthread_atfork(prepare, parent, child) != 0) {
        perror("pthread_atfork");
        exit(EXIT_FAILURE);
    }

    pthread_create(&t, NULL, thread_func, NULL);
    sleep(1); // Let the thread lock the mutex

    printf("Main thread: calling fork()\n");
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Child: running after fork\n");
   //     int ret = pthread_mutex_trylock(&lock);
   //     printf("CHILD: mutex locked: %d", ret);
    } else {
        printf("Parent: forked child with PID %d\n", pid);
    }

    pthread_join(t, NULL);
    return 0;
}