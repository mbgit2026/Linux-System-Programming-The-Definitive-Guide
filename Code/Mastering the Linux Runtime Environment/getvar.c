#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage %s <env_var>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *value;
    value = getenv(argv[1]);
    printf("\nVAR: %s=%s", argv[1], value);

    exit(EXIT_SUCCESS);

}