#define _GNU_SOURCE
#include <grp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFLEN 4096

int main(void)
{
    struct group grp;
    struct group *grpp;
    char buf[BUFLEN];
    int i;

    setgrent();
    while (1) {
        i = getgrent_r(&grp, buf, sizeof(buf), &grpp);
        if (i)
            break;
        printf("%s (%jd):", grpp->gr_name, (intmax_t) grpp->gr_gid);
        for (size_t j = 0; ; j++) {
            if (grpp->gr_mem[j] == NULL)
                break;
            printf(" %s", grpp->gr_mem[j]);
        }
        printf("\n");
    }
    endgrent();
    exit(EXIT_SUCCESS);
}
