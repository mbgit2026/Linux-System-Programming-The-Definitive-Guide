#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/random.h>

#define LENGTH 12

int main (int argc, char *argv[]) {

    char buf[LENGTH], buf2[LENGTH];
    int ret;

    if ((ret = getrandom(buf, LENGTH, GRND_NONBLOCK)) == -1) {
        perror("getrandom");
        exit(EXIT_FAILURE);
    }

    printf("Random bytes from getrandom:\n");
    for (size_t i = 0; i < LENGTH; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n\n");

    if ((ret = getentropy(buf2, LENGTH)) != 0) {
        perror("getentropy");
        exit(EXIT_FAILURE);
    }

    printf("Random bytes from getentropy:\n");
    for (size_t i = 0; i < LENGTH; i++) {
        printf("%02x ", buf2[i]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}