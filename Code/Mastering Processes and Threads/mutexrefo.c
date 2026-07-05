#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

struct foo {
    int f_count;
    pthread_mutex_t f_lock;
    int f_id;
};

struct foo *foo_alloc(int id) /* allocate the object */
{
    struct foo *fp;
    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->f_count = 1;
        fp->f_id = id;
        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            free(fp);
            return NULL;
        }
    }
    return fp;
}

void foo_hold(struct foo *fp) /* add a reference */
{
    pthread_mutex_lock(&fp->f_lock);
    fp->f_count++;
    printf("Hold: count = %d\n", fp->f_count);
    pthread_mutex_unlock(&fp->f_lock);
    sleep(5);
}

void foo_rele(struct foo *fp) /* release a reference */
{
    pthread_mutex_lock(&fp->f_lock);
    if (--fp->f_count == 0) { /* last reference */
        pthread_mutex_unlock(&fp->f_lock);
        pthread_mutex_destroy(&fp->f_lock);
        printf("Object destroyed\n");
        free(fp);
    } else {
        printf("Release: count = %d\n", fp->f_count);
        pthread_mutex_unlock(&fp->f_lock);
    }
}

/* Thread function */
void *worker(void *arg)
{
    struct foo *fp = (struct foo *)arg;

    foo_hold(fp);   // increase reference
    foo_rele(fp);   // release reference

    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;

    struct foo *fp = foo_alloc(42);
    if (fp == NULL) {
        printf("Allocation failed\n");
        return 1;
    }

    /* Create threads */
    pthread_create(&t1, NULL, worker, fp);
    pthread_create(&t2, NULL, worker, fp);
    pthread_create(&t3, NULL, worker, fp);
    /* Wait for threads */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    /* Release initial reference */
    foo_rele(fp);

    return 0;
}