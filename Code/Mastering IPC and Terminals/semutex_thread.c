#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>

struct slock {
    sem_t *semp;
    char name[NAME_MAX];
};

struct slock *s_alloc()
{
    struct slock *sp;
    static int cnt;

    if ((sp = malloc(sizeof(struct slock))) == NULL)
        return(NULL);
    
    do {
        snprintf(sp->name, sizeof(sp->name), "/%ld.%d", (long)getpid(), cnt++);
        sp->semp = sem_open(sp->name, O_CREAT|O_EXCL, S_IRWXU, 1);
    } while ((sp->semp == SEM_FAILED) && (errno == EEXIST));
    
    if (sp->semp == SEM_FAILED) {
        free(sp);
        return(NULL);
    }
    
    sem_unlink(sp->name);
    return(sp);
}

void s_free(struct slock *sp)
{
    sem_close(sp->semp);
    free(sp);
}

int s_lock(struct slock *sp)
{
    return(sem_wait(sp->semp));
}

int s_trylock(struct slock *sp)
{
    return(sem_trywait(sp->semp));
}

int s_unlock(struct slock *sp)
{
    return(sem_post(sp->semp));
}

static void *thread_func(void *arg) {

    struct slock *lock = (struct slock *)arg;
    printf("Thread %lu: Trying to acquire lock...\n", pthread_self());

    if (s_lock(lock) == -1) {
        perror("s_lock");
        pthread_exit(NULL);
    }

    printf("Thread %lu: Lock acquired. Working...\n", pthread_self());
    sleep(2); 
    printf("Thread %lu: Releasing lock...\n", pthread_self());

    if (s_unlock(lock) == -1) 
        perror("s_unlock");
    
    pthread_exit(NULL);
}



int main() {
    struct slock *lock = s_alloc();
    if (lock == NULL) {
        perror("s_alloc");
        exit(EXIT_FAILURE);
    }

    pthread_t t1;
    pthread_t t2;


    pthread_create(&t1, NULL, &thread_func, lock);
    pthread_create(&t1, NULL, &thread_func, lock);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    
    s_free(lock);

    return 0;
}