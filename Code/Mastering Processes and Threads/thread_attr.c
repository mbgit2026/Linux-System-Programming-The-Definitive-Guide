#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static void display_pthread_attr(pthread_attr_t *attr, char *prefix)
{
    int s, i;
    size_t v;
    void *stkaddr;
    struct sched_param sp;

    s = pthread_attr_getdetachstate(attr, &i);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getdetachstate: %s", strerror(s));
    printf("%sDetach state        = %s\n", prefix,
            (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
            (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
            "???");

    s = pthread_attr_getscope(attr, &i);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getscope: %s", strerror(s));
    printf("%sScope               = %s\n", prefix,
            (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
            (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
            "???");

    s = pthread_attr_getinheritsched(attr, &i);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getinheritsched: %s", strerror(s));
    printf("%sInherit scheduler   = %s\n", prefix,
            (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
            (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
            "???");

    s = pthread_attr_getschedpolicy(attr, &i);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getschedpolicy: %s", strerror(s));

    printf("%sScheduling policy   = %s\n", prefix,
            (i == SCHED_OTHER) ? "SCHED_OTHER" :
            (i == SCHED_FIFO)  ? "SCHED_FIFO" :
            (i == SCHED_RR)    ? "SCHED_RR" :
            "???");

    s = pthread_attr_getschedparam(attr, &sp);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getschedparam: %s", strerror(s));
    printf("%sScheduling priority = %d\n", prefix, sp.sched_priority);

    s = pthread_attr_getguardsize(attr, &v);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getguardsize: %s", strerror(s));
    printf("%sGuard size          = %zu bytes\n", prefix, v);

    s = pthread_attr_getstack(attr, &stkaddr, &v);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_attr_getstack: %s", strerror(s));
    printf("%sStack address       = %p\n", prefix, stkaddr);
    printf("%sStack size          = %#zx bytes\n", prefix, v);
}

static void *thread_start(void *arg)
{
    int s;
    pthread_attr_t gattr;

    s = pthread_getattr_np(pthread_self(), &gattr);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_getattr_np: %s", strerror(s));

    printf("Thread attributes:\n");
    display_pthread_attr(&gattr, "\t");

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    pthread_t thr;
    pthread_attr_t attr;
    pthread_attr_t *attrp;
    int s;

    attrp = NULL;

    if (argc > 1) {
        size_t stack_size;
        void *sp;

        attrp = &attr;

        s = pthread_attr_init(&attr);
        if (s != 0)
            errx(EXIT_FAILURE, "pthread_attr_init: %s", strerror(s));

        s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if (s != 0)
            errx(EXIT_FAILURE, "pthread_attr_setdetachstate: %s", strerror(s));

        s = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (s != 0)
            errx(EXIT_FAILURE, "pthread_attr_setinheritsched: %s", strerror(s));

        stack_size = strtoul(argv[1], NULL, 0);

        s = posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size);
        if (s != 0)
            errx(EXIT_FAILURE, "posix_memalign: %s", strerror(s));

        printf("posix_memalign() allocated at %p\n", sp);

        s = pthread_attr_setstack(&attr, sp, stack_size);
        if (s != 0)
            errx(EXIT_FAILURE, "pthread_attr_setstack: %s", strerror(s));
    }

    s = pthread_create(&thr, attrp, &thread_start, NULL);
    if (s != 0)
        errx(EXIT_FAILURE, "pthread_create: %s", strerror(s));

    if (attrp != NULL) {
        s = pthread_attr_destroy(attrp);
        if (s != 0)
            errx(EXIT_FAILURE, "pthread_attr_destroy: %s", strerror(s));
    }

    pause();
}