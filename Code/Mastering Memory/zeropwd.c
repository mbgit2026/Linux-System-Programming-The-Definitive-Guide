#include <stdio.h>
#include <string.h>
#include <strings.h>  
#include <unistd.h>

int main() {
    char password[128];

    printf("Enter password: ");
    if (!fgets(password, sizeof(password), stdin)) {
        fprintf(stderr, "Error reading password\n");
        return 1;
    }

    password[strcspn(password, "\n")] = '\0';

    printf("Password length: %zu\n", strlen(password));

    /* ... code for autentication ... */

    bzero(password, sizeof(password));
    printf("Password buffer cleared from memory.\n");

    return 0;
}
