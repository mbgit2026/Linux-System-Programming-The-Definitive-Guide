#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct shm_counter {
    int counter;
};

int main(void)
{
    int fd = shm_open("/shm_counter", O_CREAT | O_RDWR, 0600);
    ftruncate(fd, sizeof(struct shm_counter));

    struct shm_counter *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    shmp->counter = 0;

    printf("Counter initialized to %d\n", shmp->counter);
    sleep(30);  
    printf("Counter before exiting: %d\n", shmp->counter);
    shm_unlink("/shm_counter");
    return 0;
}
