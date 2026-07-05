#include <stdio.h>
#include <stdlib.h>
#include <sys/vfs.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <mount_point>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *path = argv[1];
    struct statfs fsinfo;

    if (statfs(path, &fsinfo) != 0) {
        fprintf(stderr, "statfs failed for %s: %s\n", path, strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Filesystem info for: %s\n", path);
    printf("----------------------------------------\n");
    printf("Type:              0x%lX\n", fsinfo.f_type);
    printf("Block size:        %lu bytes\n", fsinfo.f_bsize);
    printf("Total blocks:      %lu\n", fsinfo.f_blocks);
    printf("Free blocks:       %lu\n", fsinfo.f_bfree);
    printf("Available blocks:  %lu\n", fsinfo.f_bavail);
    printf("Total files:       %lu\n", fsinfo.f_files);
    printf("Free file nodes:   %lu\n", fsinfo.f_ffree);
    printf("Max name length:   %lu\n", fsinfo.f_namelen);

    return EXIT_SUCCESS;
}
