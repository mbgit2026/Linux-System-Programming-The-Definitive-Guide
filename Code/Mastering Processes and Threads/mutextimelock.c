#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

pthread_mutex_t mtx;
int counter = 0;

/* Return codes for handler */
#define HANDLE_OK        0
#define HANDLE_RETRY     1
#define HANDLE_ERROR    -1

/* Utility: build absolute timeout */
void make_timeout(struct timespec *ts, int seconds)
{
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += seconds;
}

int handle_lock_result(int ret)
{
    if (ret == 0) {
        return HANDLE_OK;

    } else if (ret == EOWNERDEAD) {
        printf("Handler: detected EOWNERDEAD\n");
        printf("Handler: state may be inconsistent (counter = %d)\n", counter);
        pthread_mutex_consistent(&mtx);
        printf("Handler: state marked consistent\n");
        return HANDLE_OK;  // mutex is now usable

    } else if (ret == ETIMEDOUT) {
        printf("Handler: timeout, retry needed\n");
        return HANDLE_RETRY;

    } else {
        printf("Handler: error %d\n", ret);
        return HANDLE_ERROR;
    }
}

void *worker1(void *arg)
{
    for (int i = 0; i < 3; i++) {
        int retries = 3;
        while (retries--) {
            struct timespec ts;
            make_timeout(&ts, 2);

            int ret = pthread_mutex_timedlock(&mtx, &ts);
            int action = handle_lock_result(ret);

            if (action == HANDLE_OK) {
                counter++;
                printf("Worker1 incremented counter to %d\n", counter);
                pthread_mutex_unlock(&mtx);
                break;
            } else if (action == HANDLE_RETRY) {
                continue;
            } else {
                break;
            }
        }
        sleep(1);
    }
    return NULL;
}

void *worker2(void *arg)
{
    struct timespec ts;
    make_timeout(&ts, 2);

    int ret = pthread_mutex_timedlock(&mtx, &ts);
    int action = handle_lock_result(ret);

    if (action == HANDLE_OK) {
        counter++;
        printf("Worker2 incremented counter to %d and will crash\n", counter);
        pthread_exit(NULL); // simulate crash
    } else {
        printf("Worker2: could not proceed\n");
    }
    return NULL;
}

void *worker3(void *arg)
{
    sleep(1);
    int retries = 5;
    while (retries--) {
        struct timespec ts;
        make_timeout(&ts, 2);

        printf("Worker3: trying timedlock...\n");
        int ret = pthread_mutex_timedlock(&mtx, &ts);
        int action = handle_lock_result(ret);

        if (action == HANDLE_OK) {
            counter++;
            printf("Worker3 incremented counter to %d\n", counter);
            pthread_mutex_unlock(&mtx);
            return NULL;
        } else if (action == HANDLE_RETRY) {
            continue;
        } else {
            return NULL;
        }
    }
    printf("Worker3: gave up after retries\n");
    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    pthread_mutex_init(&mtx, &attr);
    pthread_mutexattr_destroy(&attr);

    pthread_create(&t1, NULL, worker1, NULL);
    sleep(1);
    pthread_create(&t2, NULL, worker2, NULL);
    pthread_create(&t3, NULL, worker3, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("\nFinal counter value: %d\n", counter);
    pthread_mutex_destroy(&mtx);
    return 0;
}