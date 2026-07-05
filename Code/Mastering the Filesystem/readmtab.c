#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mntent.h>

int main(void) {
    FILE *fp;
    struct mntent *mnt;

    fp = setmntent("/etc/mtab", "r");
    if (!fp) {
        perror("setmntent");
        return EXIT_FAILURE;
    }

    printf("%-20s %-15s %-8s %-20s %-5s %-6s\n", "FSname", "Dir", "Type", "Opts", "Freq", "Passno");
    printf("--------------------------------------------------------------------------------\n");

    while ((mnt = getmntent(fp)) != NULL) {
        printf("%-20s %-15s %-8s %-20s %-5d %-6d\n",
               mnt->mnt_fsname, mnt->mnt_dir, mnt->mnt_type,
               mnt->mnt_opts, mnt->mnt_freq, mnt->mnt_passno);

        if (strcmp(mnt->mnt_dir, "/") == 0) {
            if (hasmntopt(mnt, "rw")) {
                printf("# Root filesystem is mounted read-write.\n");
            } else {
                printf("# Root filesystem is NOT mounted read-write.\n");
            }
        }
    }

    endmntent(fp);
    return EXIT_SUCCESS;
}

