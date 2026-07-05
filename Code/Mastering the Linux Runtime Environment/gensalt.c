#define _GNU_SOURCE
#include <stdio.h>
#include <crypt.h>  
#include <stdlib.h>
#include <sys/random.h>

#define SALT_BYTES 16

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("\nUsage: %s <password>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *password = argv[1];
    unsigned char random_bytes[SALT_BYTES];

    if (getrandom(random_bytes, SALT_BYTES, GRND_NONBLOCK) < 0) {
        perror("urandom");
        return 1;
    }

    char *salt = crypt_gensalt("$6$", 10000, (const char *)random_bytes, SALT_BYTES);
    if (salt == NULL) {
        perror("crypt_gensalt");
        exit(EXIT_FAILURE);
    }

    printf("Generated salt: %s\n", salt);

    char *hash = crypt(password, salt);
    if (hash == NULL) {
        perror("crypt");
        exit(EXIT_FAILURE);
    }

    printf("Password: %s\n", password);
    printf("Hash: %s\n", hash);

    exit(EXIT_SUCCESS);
}
