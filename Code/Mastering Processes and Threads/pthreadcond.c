#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

int x = 0, y = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Thread that waits until x > y
void* waiter(void* arg) {
    pthread_mutex_lock(&mut);

    printf("Waiter: Waiting for x > y...\n");
    while (x <= y) {
        pthread_cond_wait(&cond, &mut);
    }

    printf("Waiter: Condition met! x = %d, y = %d\n", x, y);
    pthread_mutex_unlock(&mut);
    return NULL;
}

// Thread that modifies x and y
void* modifier(void* arg) {
    sleep(2); // simulate work

    pthread_mutex_lock(&mut);
    printf("Modifier: Modifying x and y...\n");
    x = 5;
    y = 3;
    printf("Modifier: x = %d, y = %d\n", x, y);
    if (x > y) {
        printf("Modifier: Signaling condition\n");
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mut);
    return NULL;
}

// Thread demonstrating timed wait
void* timed_waiter(void* arg) {
    struct timeval now;
    struct timespec timeout;
    int retcode = 0;

    pthread_mutex_lock(&mut);
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 5;
    timeout.tv_nsec = now.tv_usec * 1000;

    printf("Timed Waiter: Waiting with 5-second timeout...\n");
    while (x <= y && retcode != ETIMEDOUT) {
        retcode = pthread_cond_timedwait(&cond, &mut, &timeout);
    }

    if (retcode == ETIMEDOUT) {
        printf("Timed Waiter: Timeout occurred! x = %d, y = %d\n", x, y);
    } else {
        printf("Timed Waiter: Condition met! x = %d, y = %d\n", x, y);
    }
    pthread_mutex_unlock(&mut);
    return NULL;
}

int main() {
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, waiter, NULL);
    pthread_create(&t2, NULL, modifier, NULL);
    pthread_create(&t3, NULL, timed_waiter, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
    return 0;
}