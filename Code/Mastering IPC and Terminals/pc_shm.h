#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


#define BUF_SIZE 10

struct shmseg {
    int buffer[BUF_SIZE];
    int in;
    int out;
    sem_t empty;   // free slots
    sem_t full;    // filled slots
    sem_t mutex;   // mutual exclusion
};