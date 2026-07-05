#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <file_pwd>", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct passwd *pw;
    FILE *passwd_file;

    passwd_file = fopen(argv[1], "r");
    if (passwd_file == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    printf("=== Reading %s using fgetpwent() ===\n", argv[1]);

    while ((pw = fgetpwent(passwd_file)) != NULL) {
        printf("Username: %s\n", pw->pw_name);
        printf("UID: %d\n", pw->pw_uid);
        printf("GID: %d\n", pw->pw_gid);
        printf("Home Directory: %s\n", pw->pw_dir);
        printf("Shell: %s\n", pw->pw_shell);
        printf("------------------------\n");
    }

    fclose(passwd_file);
    printf("=== Done ===\n");
 
    return 0;
}
