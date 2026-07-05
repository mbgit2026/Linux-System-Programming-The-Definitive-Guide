#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>

pthread_rwlock_t rwlock;
int shared_data = 0;

/* ---------- Utility: timestamp ---------- */
void print_time() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);
}

/* ---------- Reader: tryrdlock ---------- */
void* reader_try(void* arg) {
    int id = *(int*)arg;

    while (1) {
        print_time();

        if (pthread_rwlock_tryrdlock(&rwlock) == 0) {
            printf("[Reader %d - try] read value: %d\n", id, shared_data);
            pthread_rwlock_unlock(&rwlock);
        } else {
            printf("[Reader %d - try] could NOT read (writer active)\n", id);
        }
        sleep(1);
    }
}

/* ---------- Reader: timedrdlock ---------- */
void* reader_timed(void* arg) {
    int id = *(int*)arg;

    while (1) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 2;

        print_time();
        printf("[Reader %d - timed] trying to read...\n", id);

        int ret = pthread_rwlock_timedrdlock(&rwlock, &ts);
        print_time();
        if (ret == 0) {
            printf("[Reader %d - timed] read value: %d\n", id, shared_data);
            pthread_rwlock_unlock(&rwlock);
        } else if (ret == ETIMEDOUT) {
            printf("[Reader %d - timed] timeout (waited 2s)\n", id);
        } else {
            printf("[Reader %d - timed] error: %d\n", id, ret);
        }
        sleep(1);
    }
}

/* ---------- Reader: blocking ---------- */
void* reader_blocking(void* arg) {
    int id = *(int*)arg;

    while (1) {
        print_time();
        printf("[Reader %d - blocking] waiting...\n", id);

        pthread_rwlock_rdlock(&rwlock);
        print_time();
        printf("[Reader %d - blocking] read value: %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);

        sleep(1);
    }
}

/* ---------- Writer ---------- */
void* writer(void* arg) {
    (void)arg;

    while (1) {
        sleep(3);

        print_time();
        printf("[Writer] trying to acquire write lock...\n");

        pthread_rwlock_wrlock(&rwlock);
        int new_value = rand() % 100;
        print_time();
        printf("[Writer] writing value: %d\n", new_value);

        shared_data = new_value;
        sleep(3);

        print_time();
        printf("[Writer] done writing\n\n");
        pthread_rwlock_unlock(&rwlock);
    }
}

/* ---------- Main ---------- */
int main(int argc, char* argv[]) {
    pthread_t t_try, t_timed, t_block, t_writer;
    int id_try = 1, id_timed = 2, id_block = 3;
    int use_try = 0, use_timed = 0, use_block = 0;

    srand(time(NULL));
    pthread_rwlock_init(&rwlock, NULL);

    /* ---- Parse arguments ---- */
    if (argc == 1) {
        use_try = use_timed = use_block = 1;
    } else {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "try") == 0) use_try = 1;
            else if (strcmp(argv[i], "timed") == 0) use_timed = 1;
            else if (strcmp(argv[i], "block") == 0) use_block = 1;
            else {
                printf("Unknown option: %s\n", argv[i]);
                printf("Usage: %s [block] [try] [timed]\n", argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }

    /* ---- Create threads ---- */
    if (use_try)
        pthread_create(&t_try, NULL, reader_try, &id_try);
    if (use_timed)
        pthread_create(&t_timed, NULL, reader_timed, &id_timed);
    if (use_block)
        pthread_create(&t_block, NULL, reader_blocking, &id_block);

    /* Writer always runs */
    pthread_create(&t_writer, NULL, writer, NULL);
    /* ---- Join threads ---- */
    if (use_try)
        pthread_join(t_try, NULL);
    if (use_timed)
        pthread_join(t_timed, NULL);
    if (use_block)
        pthread_join(t_block, NULL);
    pthread_join(t_writer, NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}