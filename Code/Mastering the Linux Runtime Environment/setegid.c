#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <pwd.h>


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <username>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *username = argv[1];
    gid_t new_gid;
    struct group *new_group;
    gid_t orig_egid = getegid();
    gid_t groups[NGROUPS_MAX];
    int i;

    struct passwd *pw = getpwnam(username);
    if (initgroups(username, pw->pw_gid) != 0) {
        perror("initgroups");
        exit(EXIT_FAILURE);
    }
 	int ngroups = getgroups(NGROUPS_MAX, groups);

    printf("Original group:    %4u \t %12s \t\t", orig_egid, getgrgid(orig_egid)->gr_name);

    for (i=0; i < ngroups; i++) {
        new_gid = groups[i];
        new_group = getgrgid(new_gid);
        if (setegid(new_gid) != 0)
            printf("\nCannot switch to group %u", new_gid);
        else
            printf("\nSwitched to group: %4u \t %12s \t\tOK", new_gid, new_group->gr_name);
    }

    printf("\n\nRestoring EGID to original (%u)", orig_egid);
    if (setegid(orig_egid) != 0) {
        perror("setegid (restore)");
        return EXIT_FAILURE;
    } else {
        printf("\nExit...");
    }

    exit(EXIT_SUCCESS);
}
