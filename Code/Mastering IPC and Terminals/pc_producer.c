
#include "pc_shm.h"

int main() {
    int fd = shm_open("/pcshm", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, sizeof(struct shmseg));

    struct shmseg *shm = mmap(NULL, sizeof(*shm),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);

    shm->in = 0;
    shm->out = 0;

    sem_init(&shm->empty, 1, BUF_SIZE);
    sem_init(&shm->full, 1, 0);
    sem_init(&shm->mutex, 1, 1);

    for (int i = 1; i <= 20; i++) {
        sem_wait(&shm->empty);   // wait for space
        sem_wait(&shm->mutex);   // enter critical section

        shm->buffer[shm->in] = i;
        printf("Produced: %d\n", i);
        shm->in = (shm->in + 1) % BUF_SIZE;

        sem_post(&shm->mutex);   // leave critical section
        sem_post(&shm->full);    // signal item available

        sleep(1);
    }

    return 0;
}