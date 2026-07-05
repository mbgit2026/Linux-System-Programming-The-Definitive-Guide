#include <stdio.h>
#include <stdlib.h>
#include <fstab.h>

void print_fstab_entry(const struct fstab *fs) {
    if (!fs) return;
    printf("Spec: %-20s  File: %-15s  Vfstype: %-8s  Mntops: %-10s  Freq: %d  Passno: %d\n",
           fs->fs_spec, fs->fs_file, fs->fs_vfstype, fs->fs_mntops, fs->fs_freq, fs->fs_passno);
}

int main(void) {
    struct fstab *fs;

    printf("=== Reading /etc/fstab using getfsent() ===\n");
    if (setfsent() == 0) {
        perror("setfsent");
        exit(EXIT_FAILURE);
    }

    while ((fs = getfsent()) != NULL) {
        print_fstab_entry(fs);
    }
    endfsent();

    printf("\n=== Searching for / (root filesystem) ===\n");
    fs = getfsfile("/");
    if (fs) {
        print_fstab_entry(fs);
    } else {
        printf("Mount point '/' not found in /etc/fstab\n");
    }

    printf("\n=== Searching for /dev/sda1 ===\n");
    fs = getfsspec("/dev/sda1");
    if (fs) {
        print_fstab_entry(fs);
    } else {
        printf("Device '/dev/sda1' not found in /etc/fstab\n");
    }

    exit(EXIT_SUCCESS);
}
