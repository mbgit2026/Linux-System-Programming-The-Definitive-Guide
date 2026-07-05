#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

static void *thread_func(void *ignored_argument)
{
    int s;

    /* Disable cancelation for a while, so that we don't
        immediately react to a cancelation request.  */

    s = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_setcancelstate: %s", s);

    printf("%s(): started; cancelation disabled\n", __func__);
    sleep(5);
    printf("%s(): about to enable cancelation\n", __func__);

    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_setcancelstate: %s", s);

    /* sleep() is a cancelation point.  */

    sleep(1000);        /* Should get canceled while we sleep */

    /* Should never get here.  */

    printf("%s(): not canceled!\n", __func__);
    return NULL;
}

int main(void)
{
    pthread_t thr;
    void *res;
    int s;

    /* Start a thread and then send it a cancelation request.  */

    s = pthread_create(&thr, NULL, &thread_func, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create: %s", s);
    sleep(2);           /* Give thread a chance to get started */

    printf("%s(): sending cancelation request\n", __func__);
    s = pthread_cancel(thr);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_cancel: %s", s);

    /* Join with thread to see what its exit status was.  */

    s = pthread_join(thr, &res);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_join: %s", s);

    if (res == PTHREAD_CANCELED)
        printf("%s(): thread was canceled\n", __func__);
    else
        printf("%s(): thread wasn't canceled (shouldn't happen!)\n",
                __func__);
    exit(EXIT_SUCCESS);
}
