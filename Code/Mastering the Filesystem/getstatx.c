#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <bits/statx.h>
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
              STATX_ALL | STATX_BTIME, &stx) == -1) {
        perror("statx");
        return EXIT_FAILURE;
    }

    printf("Information for: %s\n", argv[1]);
    printf("Size: %lld bytes\n", (long long)stx.stx_size);
    printf("Mode: %o\n", stx.stx_mode & 07777);
    printf("UID: %u\n", stx.stx_uid);
    printf("GID: %u\n", stx.stx_gid);
    printf("Inode: %llu\n", (unsigned long long)stx.stx_ino);
    printf("Device ID: %u:%u\n", stx.stx_dev_major, stx.stx_dev_minor);
    printf("Hard links: %u\n", stx.stx_nlink);

    printf("Access time:\n");
    print_timestamp(&stx.stx_atime);
    printf("Modify time:\n");
    print_timestamp(&stx.stx_mtime);
    printf("Change time:\n");
    print_timestamp(&stx.stx_ctime);
    printf("Birth (creation) time:\n");
    print_timestamp(&stx.stx_btime);

    return EXIT_SUCCESS;
}


