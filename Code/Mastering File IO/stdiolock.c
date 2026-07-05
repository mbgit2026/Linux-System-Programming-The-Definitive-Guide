#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define ITER 100

FILE *fp;

void *thread_with_lock(void *arg) {
    for (int i = 0; i < ITER; i++) {
        flockfile(fp);   // lock FILE*
        fprintf(fp, "[LOCKED] ");
        fprintf(fp, "Thread A, ");
        fprintf(fp, "iteration %d\n", i);
        funlockfile(fp); // unlock FILE*
        usleep(100000);  // sleep 100ms
    }
    return NULL;
}

void *thread_without_lock(void *arg) {
    for (int i = 0; i < ITER; i++) {
        // no flockfile, so possible interleaving
    fprintf(fp, "[UNLOCKED] ");
    fprintf(fp, "Thread B, ");
    fprintf(fp, "iteration %d\n", i);
        usleep(100000);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    fp = fopen("output.txt", "w");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    pthread_create(&t1, NULL, thread_with_lock, NULL);
    pthread_create(&t2, NULL, thread_without_lock, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    fclose(fp);
    printf("Done. Check output.txt\n");
    return 0;
}
