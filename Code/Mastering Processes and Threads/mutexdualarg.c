#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

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
        pthread_mutex_unlock(&fp->f_lock);
    }
}

/* Worker thread */
void *worker(void *arg)
{
    int *obj_ids = (int *)arg;
    int obj_id = obj_ids[rand() % obj_ids[0] + 1]; // obj_ids[0] stores number of objects

    struct foo *fp = foo_find(obj_id);
    if (fp != NULL) {
        usleep(100000);  // simulate work (0.1s)
        foo_rele(fp);
    } else {
        printf("Thread: object not found (id=%d)\n", obj_id);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("Usage: %s <num_objects> <num_threads>\n", argv[0]);
        return 1;
    }

    int num_objects = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    if (num_objects <= 0 || num_threads <= 0) {
        printf("Invalid arguments.\n");
        return 1;
    }

    srand(time(NULL));

    int *obj_ids = malloc(sizeof(int) * (num_objects + 1));
    obj_ids[0] = num_objects;  // store number of objects in first element

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);

    // Create objects
    for (int i = 0; i < num_objects; i++) {
        int id = 100 + i; // assign arbitrary IDs
        obj_ids[i + 1] = id;
        struct foo *fp = foo_alloc(id);
        if (fp == NULL) {
            printf("Allocation failed for id=%d\n", id);
            return 1;
        }
        printf("Object created with id = %d\n", id);
    }
    printf("\n");

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker, obj_ids);
    }

    // Wait for threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Release all remaining references
    for (int i = 0; i < num_objects; i++) {
        struct foo *fp = foo_find(obj_ids[i + 1]);
        if (fp != NULL) {
            foo_rele(fp);
        }
    }

    free(obj_ids);
    free(threads);
    return 0;
}