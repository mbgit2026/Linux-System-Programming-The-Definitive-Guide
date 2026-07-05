#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>

#define NUM_THREADS 3
#define PERIOD_MS 100  // 100 ms period for each task

// Shared resource
int shared_counter = 0;

// Spin lock
pthread_spinlock_t lock;

// Function to sleep for a precise period (high-resolution sleep)
void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
}

// Real-time task
void* real_time_task(void* arg) {
    int thread_id = *((int*)arg);

    // Set real-time priority
    struct sched_param param;
    param.sched_priority = 80 - thread_id;  // Different priority for each thread
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param) != 0) {
        perror("Failed to set real-time priority");
    }

    for (int i = 0; i < 10; i++) {  // Run 10 periods
        // Acquire spin lock
        pthread_spin_lock(&lock);

        // Critical section
        shared_counter++;
        printf("Thread %d updated counter to %d\n", thread_id, shared_counter);

        // Release spin lock
        pthread_spin_unlock(&lock);

        // Sleep until next period
        sleep_ms(PERIOD_MS);
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize spin lock
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&threads[i], NULL, real_time_task, &thread_ids[i]);
    }

    // Join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", shared_counter);

    // Destroy spin lock
    pthread_spin_destroy(&lock);

    return 0;
}