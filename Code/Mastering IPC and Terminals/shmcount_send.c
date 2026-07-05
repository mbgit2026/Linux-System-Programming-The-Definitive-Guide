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
    int fd = shm_open("/shm_counter", O_RDWR, 0);

    struct shm_counter *shmp = mmap(NULL, sizeof(*shmp), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    for (int i = 0; i < 10; i++) {
        shmp->counter++;
        printf("Counter = %d\n", shmp->counter);
        sleep(1);
    }
    return 0;
}
