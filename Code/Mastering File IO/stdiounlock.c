#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define ITER 20

char *file;

void *thread_a(void *arg) {
    char line[64];
    for (int i = 0; i < ITER; i++) {
        snprintf(line, sizeof(line), "[Thread A] iteration %d\n", i);
        fputs(line, stdout);
        sleep(1); 
    }
    return NULL;
}

void *thread_b(void *arg) {
    FILE *f = fopen(file, "r");
    if (!f) {
        perror("fopen");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    if (size == 0) {
        fclose(f);
        return NULL;
    }

    char *buf = malloc(size + 1);
    if (!buf) {
        perror("malloc");
        fclose(f);
        return NULL;
    }

    fread_unlocked(buf, 1, size, f);
    buf[size] = '\0'; 
    fclose(f);

    usleep(1000000); 

    while (ftrylockfile(stdout) != 0) 
        usleep(200000); // 0.2s
    
    fputs("\n  [Thread B] acquired lock\n", stdout);
    fputs(buf, stdout);
    fputs("\n\n", stdout);
    funlockfile(stdout);
    usleep(500000); 

    free(buf);
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <file>", argv[0]);
        return 1;
    }

    file = argv[1];

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_a, NULL);
    pthread_create(&t2, NULL, thread_b, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
