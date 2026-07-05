#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <mntent.h>
#include <string.h>

const char *mountpoint;

struct mntent *findentry() {
    
    struct mntent *mnt = NULL;
    FILE *fp = setmntent("/etc/mtab", "r");

    if (!fp) 
        perror("setmntent");    
    
    while ((mnt = getmntent(fp)) != NULL) {
        if (strcmp(mnt->mnt_dir, mountpoint) == 0) {
            endmntent(fp);
            return mnt;
        }
    }
    return mnt;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage: %s <mount_point>", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fsfd, mfd, pickfd;
    mountpoint = argv[1];

    if (mkdir(mountpoint, 0755) == -1 && errno != EEXIST) {
        perror("mkdir mountpoint");
        return EXIT_FAILURE;
    }

    fsfd = fsopen("tmpfs", FSOPEN_CLOEXEC);
    if (fsfd == -1) {
        perror("fsopen");
        return EXIT_FAILURE;
    }
    printf("Opened tmpfs context, fd=%d\n", fsfd);

    if (fsconfig(fsfd, FSCONFIG_SET_STRING, "source", "mytmpfs", 0) == -1) {
        perror("fsconfig: source");
        close(fsfd);
        return EXIT_FAILURE;
    }

    if (fsconfig(fsfd, FSCONFIG_CMD_CREATE, NULL, NULL, 0) == -1) {
        perror("fsconfig: create");
        close(fsfd);
        return EXIT_FAILURE;
    }

    mfd = fsmount(fsfd, 0, 0);
    if (mfd == -1) {
        perror("fsmount");
        close(fsfd);
        return EXIT_FAILURE;
    }
    close(fsfd);

    if (move_mount(mfd, "", AT_FDCWD, mountpoint, MOVE_MOUNT_F_EMPTY_PATH) == -1) {
        perror("move_mount");
        close(mfd);
        return EXIT_FAILURE;
    }
    close(mfd);

    pickfd = fspick(AT_FDCWD, mountpoint, 0);
    if (pickfd == -1) {
        perror("fspick");
        return EXIT_FAILURE;
    }
    printf("\nPicked tmpfs mount from %s, fd=%d", mountpoint, pickfd);

    struct mntent *entry = findentry();
    printf("\nEntry in /etc/mtab: ");
    printf("\n%s  %s  %s   %s  %d  %d\n", entry->mnt_fsname, entry->mnt_dir, entry->mnt_type, entry->mnt_opts, entry->mnt_freq, entry->mnt_passno);
    
    printf("\nReconfiguring the fs to be read-only...");
    fsconfig(pickfd, FSCONFIG_SET_FLAG, "ro", NULL, 0);
    fsconfig(pickfd, FSCONFIG_CMD_RECONFIGURE, NULL, NULL, 0);
    entry = findentry();
    printf("\n%s  %s  %s   %s  %d  %d", entry->mnt_fsname, entry->mnt_dir, entry->mnt_type, entry->mnt_opts, entry->mnt_freq, entry->mnt_passno);

    close(pickfd);
    return EXIT_SUCCESS;
}

