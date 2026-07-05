#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *thread_func1(void *arg)
{
    printf("Thread 1: Hello from thread 1\n");
    return "Thread 1 finished";
}

void *thread_func2(void *arg)
{
    printf("Thread 2: Hello from thread 2\n");
    return "Thread 2 finished";
}

int main(void)
{
    pthread_t t1, t2;
    void *res1;
    void *res2;

    /* Create the threads */
    if (pthread_create(&t1, NULL, thread_func1, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&t2, NULL, thread_func2, NULL) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }

    /* Wait for the threads to finish */
    if (pthread_join(t1, &res1) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(t2, &res2) != 0) {
        perror("pthread_join");
        exit(EXIT_FAILURE);
    }

    /* Check return values */
    printf("Main: thread 1 returned: %s\n", (char *)res1);
    printf("Main: thread 2 returned: %s\n", (char *)res2);

    return 0;
}