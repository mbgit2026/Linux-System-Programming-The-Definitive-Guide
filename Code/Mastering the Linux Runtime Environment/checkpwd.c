#define _XOPEN_SOURCE
#include <crypt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bsd/readpassphrase.h>

int main() {
    const char *stored_hash = "$6$newsaltstr$0I05pcgWEWXk1jrSaBBs6hZLISQTtealfWT6VdeD94GYykJ010I8cKg8C08FW099g/YC/GRBL.t./Y.tus/EM0";

    char input[100];

  /*  printf("Enter password: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("fgets");
        return 1;
    }
    */

    if (readpassphrase("Password: ", input, sizeof(input), RPP_REQUIRE_TTY) == NULL)
        perror("readpassphrase");


    input[strcspn(input, "\n")] = '\0';

    char salt[100];
    const char *p = stored_hash;
    int dollar_count = 0;
    size_t i = 0;

    while (*p && dollar_count < 3 && i < sizeof(salt) - 1) {
        salt[i++] = *p;
        if (*p == '$') {
            dollar_count++;
        }
        p++;
    }
    salt[i] = '\0';

    char *hashed = crypt(input, salt);
    if (hashed == NULL) {
        perror("crypt");
        return 1;
    }

    if (strcmp(hashed, stored_hash) == 0) {
        printf("✅ Password correct!\n");
    } else {
        printf("❌ Invalid password.\n");
    }

    return 0;
}
