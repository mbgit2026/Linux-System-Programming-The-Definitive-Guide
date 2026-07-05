#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio_ext.h>

#define ITER 20

FILE *fp;
char *file;

void *thread_a(void *arg) {
    char line[19] = "Hello Linux World!\0";
    for (int i = 0; i < ITER; i++) {
        fputs(line, fp);
        fputs("\n", fp);
        usleep(200000);
    }
    return NULL;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "\nUsage: %s <file> <thread_num>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "w");
    if (!fp) {
        perror("fopen");
        return 1;
    }
    
    __fsetlocking(fp, FSETLOCKING_BYCALLER);

    int n = atoi(argv[2]);
    pthread_t *threads = malloc(n * sizeof(pthread_t));

    for (int i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, thread_a, NULL);
    }


    for (int i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(fp);
    free(threads);
    return 0;
}
