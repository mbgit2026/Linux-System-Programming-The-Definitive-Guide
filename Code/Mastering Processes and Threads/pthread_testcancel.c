#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *thread_func(void *arg)
{
    int s;

    printf("Thread: started\n");

    /* Disable cancellation */
    s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (s != 0) {
        perror("pthread_setcancelstate");
        exit(EXIT_FAILURE);
    }

    printf("Thread: cancellation disabled\n");

    for (int i = 0; i < 5; i++) {
        printf("Thread working (cancel disabled)...\n");
        sleep(1);
    }

    /* Enable cancellation */
    printf("Thread: enabling cancellation\n");

    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0) {
        perror("pthread_setcancelstate");
        exit(EXIT_FAILURE);
    }

    /* Explicit cancellation point */
    printf("Thread: testing for cancellation\n");
    pthread_testcancel();

    printf("Thread: this line should not execute if canceled\n");

    return "Thread finished normally";
}

int main(void)
{
    pthread_t tid;
    void *res;

    pthread_create(&tid, NULL, thread_func, NULL);

    sleep(2);

    printf("Main: sending cancellation request\n");
    pthread_cancel(tid);

    pthread_join(tid, &res);

    if (res == PTHREAD_CANCELED)
        printf("Main: thread was canceled\n");
    else
        printf("Main: thread returned: %s\n", (char *)res);

    return 0;
}