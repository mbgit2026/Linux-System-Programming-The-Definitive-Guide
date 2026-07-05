#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *username = argv[1];
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
        

    // Get current group list
    gid_t groups[NGROUPS_MAX];
    int ngroups = getgroups(NGROUPS_MAX, groups);
    if (ngroups == -1) {
        perror("getgroups");
        exit(EXIT_FAILURE);
    }

    printf("Supplementary groups (%d):\n", ngroups);
    for (int i = 0; i < ngroups; i++) {
        printf("  [%d] GID: %d - %s\n", i, groups[i], getgrgid(groups[i])->gr_name);
    }

    if (ngroups >= 1) {
        printf("Trying to reduce group list to the first one only...\n");

        if (setgroups(1, &groups[0]) == -1) {
            perror("setgroups (requires root)");
        } else {
            printf("Successfully set reduced group list.\n");

            // Get updated group list
            gid_t new_groups[NGROUPS_MAX];
            int new_ngroups = getgroups(NGROUPS_MAX, new_groups);
            if (new_ngroups == -1) {
                perror("getgroups (after setgroups)");
            } else {
                printf("New supplementary group list (%d):\n", new_ngroups);
                for (int i = 0; i < new_ngroups; i++) {
                    printf("  [%d] GID: %d - %s\n", i, new_groups[i], getgrgid(new_groups[i])->gr_name);
                }
            }
        }
    }

    exit(EXIT_SUCCESS);
}
