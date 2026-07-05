#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 2
#define NUM_NUMBERS 20

int numbers[NUM_NUMBERS];
double halves[NUM_NUMBERS];
double results[NUM_NUMBERS];

pthread_barrier_t barrier;
pthread_barrier_t bar;

typedef struct {
    int start;
    int end;
} ThreadData;

void* worker(void* arg) {
    ThreadData* data = (ThreadData*)arg;

    for (int i = data->start; i < data->end; i++) {
        halves[i] = numbers[i] / 2.0;
    }
    pthread_barrier_wait(&barrier);

    if (data->start == 0) {
        for (int i = data->start + 10; i < data->end + 10; i++) 
            results[i-10] = halves[i];
    } else if (data->start == 10) {
        for (int i = data->start - 10; i < data->end - 10; i++) 
            results[i + 10] = halves[i];
    }


    pthread_barrier_wait(&bar);

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    for (int i = 0; i < NUM_NUMBERS; i++) {
        numbers[i] = i + 1;
    }

    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    pthread_barrier_init(&bar, NULL, NUM_THREADS + 1);

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start = i * (NUM_NUMBERS / NUM_THREADS);
        thread_data[i].end = (i + 1) * (NUM_NUMBERS / NUM_THREADS);
        pthread_create(&threads[i], NULL, worker, &thread_data[i]);
    }

    pthread_barrier_wait(&bar);
    
    printf("Final results:\n");
    for (int i = 0; i < NUM_NUMBERS; i++) {
        printf("%.1f ", results[i]);
    }
    printf("\n");

    pthread_barrier_destroy(&barrier);
    pthread_barrier_destroy(&bar);
    return 0;
    
}