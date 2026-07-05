#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int            ngroups;
    gid_t          *groups;
    struct group   *gr;
    struct passwd  *pw;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <user> <ngroups>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ngroups = atoi(argv[2]);

    groups = malloc(sizeof(*groups) * ngroups);
    if (groups == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    errno = 0;
    pw = getpwnam(argv[1]);
    if (pw == NULL) {
        if (errno)
            perror("getpwnam");
        else
            fprintf(stderr, "no such user\n");
        exit(EXIT_FAILURE);
    }

    if (getgrouplist(argv[1], pw->pw_gid, groups, &ngroups) == -1) {
 		fprintf(stderr, "getgrouplist() returned -1; ngroups = %d\n", ngroups);
 		exit(EXIT_FAILURE);
 	}
/*
    if (getgrouplist(argv[1], pw->pw_gid, groups, &ngroups) == -1) {
        getgrouplist(argv[1], pw->pw_gid, groups, &ngroups);
    }
*/
    fprintf(stderr, "ngroups = %d\n", ngroups);
    for (int j = 0; j < ngroups; j++) {
        printf("%d", groups[j]);
        gr = getgrgid(groups[j]);
        if (gr != NULL)
            printf(" (%s)", gr->gr_name);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
