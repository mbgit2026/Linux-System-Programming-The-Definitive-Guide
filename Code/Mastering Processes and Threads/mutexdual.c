#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id) % NHASH)

struct foo {
    int f_count;
    pthread_mutex_t f_lock;
    int f_id;
    struct foo *f_next;
};

struct foo *fh[NHASH];
pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

/* Allocate object */
struct foo *foo_alloc(int id)
{
    struct foo *fp;
    int idx;

    if ((fp = malloc(sizeof(struct foo))) != NULL) {
        fp->f_count = 1;
        fp->f_id = id;

        if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
            free(fp);
            return NULL;
        }

        idx = HASH(id);

        printf("\nIDX: %d\n", idx);

        pthread_mutex_lock(&hashlock);
        fp->f_next = fh[idx];
        fh[idx] = fp;
        pthread_mutex_lock(&fp->f_lock);
        pthread_mutex_unlock(&hashlock);

        pthread_mutex_unlock(&fp->f_lock);
    }
    return fp;
}

/* Find object */
struct foo *foo_find(int id)
{
    struct foo *fp;

    pthread_mutex_lock(&hashlock);
    for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
        if (fp->f_id == id) {
            pthread_mutex_lock(&fp->f_lock);
            fp->f_count++;
            printf("Find id %d: count = %d\n", id, fp->f_count);
            pthread_mutex_unlock(&hashlock);
            pthread_mutex_unlock(&fp->f_lock);
            return fp;
        }
    }
    pthread_mutex_unlock(&hashlock);
    return NULL;
}

/* Release reference */
void foo_rele(struct foo *fp)
{
    int idx;

    pthread_mutex_lock(&fp->f_lock);
    if (--fp->f_count == 0) {
        pthread_mutex_unlock(&fp->f_lock);

        pthread_mutex_lock(&hashlock);
        pthread_mutex_lock(&fp->f_lock);

        if (fp->f_count != 0) {
            pthread_mutex_unlock(&fp->f_lock);
            pthread_mutex_unlock(&hashlock);
            return;
        }

        idx = HASH(fp->f_id);
        struct foo *tfp = fh[idx];

        if (tfp == fp) {
            fh[idx] = fp->f_next;
        } else {
            while (tfp->f_next != fp)
                tfp = tfp->f_next;
            tfp->f_next = fp->f_next;
        }

        pthread_mutex_unlock(&hashlock);
        pthread_mutex_unlock(&fp->f_lock);

        pthread_mutex_destroy(&fp->f_lock);
        printf("Object destroyed: %d\n", fp->f_id);
        free(fp);
    } else {
        printf("Release: count = %d\n", fp->f_count);
        pthread_mutex_unlock(&fp->f_lock);
    }
}

void *worker(void *arg)
{
    int id = *(int *)arg;

    struct foo *fp = foo_find(id);
    if (fp != NULL) {
        sleep(1);          // simulate work
        foo_rele(fp);
    } else {
        printf("Thread: object not found\n");
    }

    return NULL;
}

int main()
{
    pthread_t t1, t2, t3;
    int id = 42;

    struct foo *fp = foo_alloc(id);
    if (fp == NULL) {
        printf("Allocation failed\n");
        return 1;
    }
    printf("Object created with id = %d\n", id);

    pthread_create(&t1, NULL, worker, &id);
    pthread_create(&t2, NULL, worker, &id);
    pthread_create(&t3, NULL, worker, &id);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    foo_rele(fp);
    return 0;
    
}