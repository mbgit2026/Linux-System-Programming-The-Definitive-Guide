#include "pc_shm.h"

int main() {
    int fd = shm_open("/pcshm", O_RDWR, 0);

    struct shmseg *shm = mmap(NULL, sizeof(*shm),
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);

    for (int i = 1; i <= 20; i++) {
        sem_wait(&shm->full);
        sem_wait(&shm->mutex);

        int item = shm->buffer[shm->out];
        printf("Consumed: %d\n", item);

        shm->out = (shm->out + 1) % BUF_SIZE;

        sem_post(&shm->mutex);
        sem_post(&shm->empty);
        sleep(2);
    }

    shm_unlink("/pcshm");
    return 0;
}