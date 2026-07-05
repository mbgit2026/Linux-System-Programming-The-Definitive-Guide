#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>

struct shmseg {
    int request;
    int response;
    sem_t req_sem;   // client → server
    sem_t res_sem;   // server → client
};