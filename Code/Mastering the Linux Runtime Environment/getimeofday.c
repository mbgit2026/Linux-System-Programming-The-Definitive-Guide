#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    
    struct timeval *tv;
    int curt;

    if (gettimeofday(tv, NULL) == -1) {
        perror("gettimeofday");
        exit(EXIT_FAILURE);
    }

    printf("\nCurrent: %d", tv->tv_sec);

    if ((curt = time(NULL)) == -1) {
        perror("time");
    }

    printf("\nCurrent: %d", curt);

}