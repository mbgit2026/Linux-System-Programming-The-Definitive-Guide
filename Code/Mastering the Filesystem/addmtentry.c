#include <stdio.h>
#include <stdlib.h>
#include <mntent.h>

int main(int argc, char *argv[]) {


    if (argc != 8) {
        printf("\nUsage: %s <mtab file> <dev> <mount point> <fstype> <opts> <freq> <pass>", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    struct mntent new_entry;

    const char *filename = argv[1];

    fp = setmntent(filename, "a+");
    if (!fp) {
        perror("setmntent");
        exit(EXIT_FAILURE);
    }

    new_entry.mnt_fsname = argv[2];
    new_entry.mnt_dir = argv[3];
    new_entry.mnt_type = argv[4];
    new_entry.mnt_opts = argv[5];
    new_entry.mnt_freq = atoi(argv[6]);
    new_entry.mnt_passno = atoi(argv[7]);

    if (addmntent(fp, &new_entry) != 0) {
        perror("addmntent");
        endmntent(fp);
        exit(EXIT_FAILURE);
    }

    printf("Entry added to %s\n", filename);
    endmntent(fp);
    exit(EXIT_SUCCESS);
}
