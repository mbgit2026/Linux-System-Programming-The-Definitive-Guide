#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>


int main(int argc, char *argv[]) {


    if (argc != 2) {
        printf("\nUsage: %s <file>", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *username = "marco";
    struct passwd *pw = getpwnam(username);
    if (!pw) {
        perror("getpwnam");
        exit(EXIT_FAILURE);
    }

    printf("Initializing group list for user: %s\n", username);
    if (initgroups(username, pw->pw_gid) != 0) {
        perror("initgroups");
        exit(EXIT_FAILURE);
    }

}

