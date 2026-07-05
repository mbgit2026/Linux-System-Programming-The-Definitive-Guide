#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/quota.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filesystem> <uid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *fs_path = argv[1];
    uid_t uid = atoi(argv[2]);

    struct dqblk dq;

    memset(&dq, 0, sizeof(dq));

    if (quotactl(QCMD(Q_GETQUOTA, USRQUOTA), fs_path, uid, (caddr_t)&dq) == -1) {
        perror("quotactl");
        return EXIT_FAILURE;
    }

    printf("Disk quota for UID %d on %s:\n", uid, fs_path);
    printf("----------------------------------------\n");
    printf("Block hard limit:   %llu\n", (unsigned long long) dq.dqb_bhardlimit);
    printf("Block soft limit:   %llu\n", (unsigned long long) dq.dqb_bsoftlimit);
    printf("Current space used: %llu\n", (unsigned long long) dq.dqb_curspace);
    printf("Inode hard limit:   %llu\n", (unsigned long long) dq.dqb_ihardlimit);
    printf("Inode soft limit:   %llu\n", (unsigned long long) dq.dqb_isoftlimit);
    printf("Current inodes used:%llu\n", (unsigned long long) dq.dqb_curinodes);

    return EXIT_SUCCESS;
}
