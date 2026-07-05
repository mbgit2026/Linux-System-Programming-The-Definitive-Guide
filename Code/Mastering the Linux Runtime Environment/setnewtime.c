#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    struct timeval new_time;
    time_t t;

    if (argc != 7) {
        printf("\nUsage: %s year month day hour min sec", argv[0]);
        exit(EXIT_FAILURE);
    }

    gettimeofday(&new_time, NULL);
    t = new_time.tv_sec;
    printf("\nCurrent system time: %s", ctime(&t));

    struct tm tm_time = {
        .tm_year = atoi(argv[1]) - 1900,
        .tm_mon = atoi(argv[2]) - 1,
        .tm_mday = atoi(argv[3]),
        .tm_hour = atoi(argv[4]),
        .tm_min = atoi(argv[5]),
        .tm_sec = atoi(argv[6]),
    };

    time_t new_epoch = mktime(&tm_time); 

    if (new_epoch == -1) {
        perror("mktime failed");
        return EXIT_FAILURE;
    }

    new_time.tv_sec = new_epoch;
    new_time.tv_usec = 0;

    if (settimeofday(&new_time, NULL) == -1) {
        perror("settimeofday");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&new_time, NULL);
    t = new_time.tv_sec;
    printf("New system time: %s", ctime(&t));

    exit(EXIT_SUCCESS);
}
