
#include "cs_shm.h"

int main() {
    int fd = shm_open("/csshm", O_RDWR, 0);

    struct shmseg *shm = mmap(NULL, sizeof(*shm),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED, fd, 0);

    for (int i = 1; i <= 5; i++) {
        shm->request = i;

        printf("Client sent: %d\n", i);
        sem_post(&shm->req_sem);   // send request

        sem_wait(&shm->res_sem);   // wait for response

        printf("Client received: %d\n", shm->response);
    }

    return 0;
}