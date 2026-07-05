#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

void *thread_func(void *arg)
{
    int sleep_time = *(int *)arg;

    printf("Thread: sleeping for %d seconds\n", sleep_time);
    sleep(sleep_time);

    printf("Thread: finished execution\n");
    return (void *) "Thread done";
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <join_wait_seconds> <thread_sleep_seconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int join_wait = atoi(argv[1]);
    int thread_sleep = atoi(argv[2]);

    pthread_t tid;
    void *res;

    if (pthread_create(&tid, NULL, thread_func, &thread_sleep) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += join_wait;

    int s = pthread_timedjoin_np(tid, &res, &ts);

    if (s == 0) {
        printf("Main: thread joined successfully\n");
        printf("Main: thread returned: %s\n", (char *)res);
    } 
    else if (s == ETIMEDOUT) {
        printf("Main: pthread_timedjoin_np timed out\n");
    } 
    else {
        perror("pthread_timedjoin_np");
    }

    return 0;
}