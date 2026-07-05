#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

struct msg {
    int data;
    struct msg *m_next;
};

struct msg *workq = NULL;
pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;

void* process_msg(void *arg)
{
    struct msg *mp;

    for (;;) {
        pthread_mutex_lock(&qlock);

        while (workq == NULL) {
            printf("Consumer: Queue empty, waiting...\n");
            pthread_cond_wait(&qready, &qlock);
        }

        mp = workq;
        workq = mp->m_next;
        pthread_mutex_unlock(&qlock);
        printf("Consumer: Processing message with data = %d\n", mp->data);
        free(mp);
        sleep(1); // simulate processing time
    }
    return NULL;
}

void enqueue_msg(struct msg *mp)
{
    pthread_mutex_lock(&qlock);

    mp->m_next = workq;
    workq = mp;
    printf("Producer: Enqueued message with data = %d\n", mp->data);
    pthread_mutex_unlock(&qlock);
    pthread_cond_signal(&qready);
}

void* producer(void *arg)
{
    int i;
    for (i = 1; i <= 5; i++) {
        struct msg *mp = malloc(sizeof(struct msg));
        mp->data = i;

        enqueue_msg(mp);
        sleep(2); // simulate time between producing messages
    }
    return NULL;
}

int main()
{
    pthread_t prod, cons;

    pthread_create(&cons, NULL, process_msg, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(prod, NULL);
    sleep(5);   // Let consumer run a bit longer
    printf("Main: Exiting program\n");
    return 0;
}