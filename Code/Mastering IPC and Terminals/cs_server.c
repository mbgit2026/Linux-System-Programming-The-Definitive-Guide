
#include "cs_shm.h"

int main() {
    int fd = shm_open("/csshm", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, sizeof(struct shmseg));

    struct shmseg *shm = mmap(NULL, sizeof(*shm),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);

    sem_init(&shm->req_sem, 1, 0);
    sem_init(&shm->res_sem, 1, 0);

    while (1) {
        sem_wait(&shm->req_sem);   // wait for request

        int x = shm->request;
        printf("Server received: %d\n", x);

        shm->response = x * x;     // process request

        sem_post(&shm->res_sem);   // signal response ready
    }
    shm_unlink("/csshm");
    return 0;
}