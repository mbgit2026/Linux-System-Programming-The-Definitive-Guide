#include <stdio.h>
#include <stdlib.h>
#include <shadow.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *username = argv[1];
    struct spwd *shadow_entry;

    shadow_entry = getspnam(username);
    if (!shadow_entry) {
        printf("\nUser not found or permission error");
        exit(EXIT_FAILURE);
    }

    printf("Shadow entry for user: %s\n", username);
    printf("  Encrypted password: %s\n", shadow_entry->sp_pwdp);
    printf("  Last password change (days since epoch): %ld\n", shadow_entry->sp_lstchg);
    printf("  Min days between changes: %ld\n", shadow_entry->sp_min);
    printf("  Max days between changes: %ld\n", shadow_entry->sp_max);
    printf("  Warning period (days): %ld\n", shadow_entry->sp_warn);
    printf("  Inactive period (days): %ld\n", shadow_entry->sp_inact);
    printf("  Account expiration (days since epoch): %ld\n", shadow_entry->sp_expire);
    printf("  Reserved field: %lu\n", shadow_entry->sp_flag);

    exit(EXIT_SUCCESS);
}
