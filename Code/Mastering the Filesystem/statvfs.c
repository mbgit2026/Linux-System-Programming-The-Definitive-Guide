#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mount_point>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    struct statvfs fsinfo;

    if (statvfs(path, &fsinfo) != 0) {
        fprintf(stderr, "statvfs failed for %s: %s\n", path, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Filesystem info for: %s (via statvfs)\n", path);
    printf("------------------------------------------------\n");
    printf("Block size (f_bsize):       %lu\n", fsinfo.f_bsize);
    printf("Fragment size (f_frsize):   %lu\n", fsinfo.f_frsize);
    printf("Total blocks (f_blocks):    %lu\n", fsinfo.f_blocks);
    printf("Free blocks (f_bfree):      %lu\n", fsinfo.f_bfree);
    printf("Avail blocks (f_bavail):    %lu\n", fsinfo.f_bavail);
    printf("Total inodes (f_files):     %lu\n", fsinfo.f_files);
    printf("Free inodes (f_ffree):      %lu\n", fsinfo.f_ffree);
    printf("Filesystem ID (f_fsid):     %lu\n", fsinfo.f_fsid);
    printf("Max filename length:        %lu\n", fsinfo.f_namemax);
    printf("Filesystem flags:           %lu\n", fsinfo.f_flag);

    return EXIT_SUCCESS;
}
