#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NAMELEN 16

static void *threadfunc(void *parm)
{
    sleep(5);          // allow main program to set the thread name
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    int rc;
    char thread_name[NAMELEN];

    rc = pthread_create(&thread, NULL, threadfunc, NULL);
    if (rc != 0)
        errx(EXIT_FAILURE, "pthread_create: %s", strerror(rc));

    rc = pthread_getname_np(thread, thread_name, NAMELEN);
    if (rc != 0)
        errx(EXIT_FAILURE, "pthread_getname_np: %s", strerror(rc));

    printf("Created a thread.  Default name is: %s\n", thread_name);
    rc = pthread_setname_np(thread, (argc > 1) ? argv[1] : "THREADFOO");
    if (rc != 0)
        errx(EXIT_FAILURE, "pthread_setname_np: %s", strerror(rc));

    sleep(2);
    rc = pthread_getname_np(thread, thread_name, NAMELEN);
    if (rc != 0)
        errx(EXIT_FAILURE, "pthread_getname_np: %s", strerror(rc));
    printf("The thread name after setting it is %s.\n", thread_name);

    rc = pthread_join(thread, NULL);
    if (rc != 0)
        errx(EXIT_FAILURE, "pthread_join: %s", strerror(rc));

    printf("Done\n");
    exit(EXIT_SUCCESS);
}
