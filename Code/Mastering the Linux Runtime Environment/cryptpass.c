#define _XOPEN_SOURCE
#include <crypt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("\nUsage %s <salt> <password>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *password = argv[2];
    const char *salt = argv[1];

    char *hashed = crypt(password, salt);
    if (hashed == NULL) {
        perror("crypt");
        return 1;
    }

    printf("Password: %s\n", password);
    printf("Salt: %s\n", salt);
    printf("Hashed: %s\n", hashed);

    return 0;
}
