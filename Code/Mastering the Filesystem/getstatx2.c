#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>

void print_timestamp(const struct statx_timestamp *ts) {
    if (ts->tv_sec == 0 && ts->tv_nsec == 0)
        printf("    (not set)\n");
    else {
        char buf[64];
        time_t sec = ts->tv_sec;
        struct tm *tm = localtime(&sec);
        strftime(buf, sizeof(buf), "%F %T", tm);
        printf("    %s.%09u\n", buf, ts->tv_nsec);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct statx stx;

    if (statx(AT_FDCWD, argv[1], AT_SYMLINK_NOFOLLOW,
              STATX_ALL, &stx) == -1) {
        perror("statx");
        return EXIT_FAILURE;
    }

    printf("=== STATX FULL INFORMATION ===\n");
    printf("File: %s\n\n", argv[1]);

    /* What fields are valid */
    printf("Mask (valid fields): 0x%x\n", stx.stx_mask);
    printf("Attributes: 0x%x\n", stx.stx_attributes);
    printf("Attributes mask: 0x%x\n", stx.stx_attributes_mask);

    /* Basic identity */
    printf("\n--- Basic Info ---\n");
    printf("Inode: %llu\n", (unsigned long long)stx.stx_ino);
    printf("Size: %llu bytes\n", (unsigned long long)stx.stx_size);
    printf("Blocks: %llu\n", (unsigned long long)stx.stx_blocks);
    printf("Block size: %u\n", stx.stx_blksize);

    /* Device info */
    printf("\n--- Device Info ---\n");
    printf("Device (major:minor): %u:%u\n",
           stx.stx_dev_major, stx.stx_dev_minor);
  //  printf("Filesystem ID: %llu\n", (unsigned long long)stx.stx_dev);

    /* Ownership */
    printf("\n--- Ownership ---\n");
    printf("UID: %u\n", stx.stx_uid);
    printf("GID: %u\n", stx.stx_gid);
    printf("Links: %u\n", stx.stx_nlink);

    /* File type & permissions */
    printf("\n--- Mode ---\n");
    printf("Raw mode: %o\n", stx.stx_mode);
    printf("Permissions: %o\n", stx.stx_mode & 0777);

    /* Timestamps */
    printf("\n--- Times ---\n");
    printf("Access time:\n");
    print_timestamp(&stx.stx_atime);

    printf("Modify time:\n");
    print_timestamp(&stx.stx_mtime);

    printf("Change time:\n");
    print_timestamp(&stx.stx_ctime);

    printf("Birth time:\n");
    print_timestamp(&stx.stx_btime);

    return EXIT_SUCCESS;
}