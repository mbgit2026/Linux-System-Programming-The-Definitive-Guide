#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define SEM_NAME "/my_shared_lock"

struct slock {
    sem_t *semp;
    char name[64];
};

/* Allocate (open or create) the shared semaphore */
struct slock *s_alloc()
{
    struct slock *sp;

    if ((sp = malloc(sizeof(struct slock))) == NULL)
        return NULL;

    snprintf(sp->name, sizeof(sp->name), "%s", SEM_NAME);
    sp->semp = sem_open(sp->name, O_CREAT, 0644, 1);
    if (sp->semp == SEM_FAILED) {
        free(sp);
        return NULL;
    }

    return sp;
}

/* Free the lock */
void s_free(struct slock *sp)
{
    sem_close(sp->semp);
    free(sp);
}

/* Lock (blocking) */
int s_lock(struct slock *sp)
{
    return sem_wait(sp->semp);
}

/* Try lock (non-blocking) */
int s_trylock(struct slock *sp)
{
    return sem_trywait(sp->semp);
}

/* Unlock */
int s_unlock(struct slock *sp)
{
    return sem_post(sp->semp);
}

/* remove the sempahore */
void s_destroy()
{
    if (sem_unlink(SEM_NAME) == -1)
        perror("sem_unlink");
}

int main()
{
    struct slock *lock = s_alloc();
    if (lock == NULL) {
        perror("s_alloc");
        exit(EXIT_FAILURE);
    }

    printf("PID %d: Trying to acquire lock...\n", getpid());
    if (s_lock(lock) == -1) {
        perror("s_lock");
        s_free(lock);
        exit(EXIT_FAILURE);
    }

    printf("PID %d: Lock acquired. Entering critical section...\n", getpid());
    sleep(3);  
    printf("PID %d: Leaving critical section\n", getpid());
    if (s_unlock(lock) == -1) 
        perror("s_unlock");  

    s_free(lock);
    return 0;
}