#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>

int main() {
    struct passwd *pw;

    printf("=== Reading /etc/passwd entries ===\n");
    setpwent();

    while ((pw = getpwent()) != NULL) {
        printf("Username: %s\n", pw->pw_name);
        printf("UID: %d\n", pw->pw_uid);
        printf("GID: %d\n", pw->pw_gid);
        printf("Home Directory: %s\n", pw->pw_dir);
        printf("Shell: %s\n", pw->pw_shell);
        printf("------------------------\n");
    }

    endpwent();
    printf("=== Done ===\n");
    return 0;
}
