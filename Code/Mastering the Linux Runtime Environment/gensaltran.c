#define _GNU_SOURCE
#include <stdio.h>
#include <crypt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>

#define SALT_BYTES 16

int main() {
    const char *password = "mypassword";
    unsigned char random_bytes[SALT_BYTES];

    if (urandom(random_bytes, SALT_BYTES) < 0) {
        perror("urandom");
        return 1;
    }

    char *salt = crypt_gensalt("$6$", 10000, (const char *)random_bytes, SALT_BYTES);
    if (salt == NULL) {
        perror("crypt_gensalt");
        return 1;
    }

    printf("Generated salt: %s\n", salt);

    char *hash = crypt(password, salt);
    if (hash == NULL) {
        perror("crypt");
        return 1;
    }

    printf("Password: %s\n", password);
    printf("Hash: %s\n", hash);

    exit(EXIT_SUCCESS);
}
