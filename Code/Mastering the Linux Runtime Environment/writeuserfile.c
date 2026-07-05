#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>

int main (int argc, char *argv[]) {

    if (argc != 4) {
        printf("\nUsage: %s <filename> <username> <string>", argv[0]);
        exit(EXIT_FAILURE);
    }

    uid_t neweuid;
    int fd;
    size_t bw;
    char *buf = argv[3];
    struct passwd *pw;

    pw = getpwnam(argv[2]);
    if (!pw) {
        perror("getpwnam");
        exit(EXIT_FAILURE);
    }

    printf("\nOriginal EUID: %u  EGID: %u", geteuid(), getegid());

    if (setegid(pw->pw_gid) != 0) {
        perror("setegid");
        exit(EXIT_FAILURE);
    }
    if (seteuid(pw->pw_uid) != 0) {
        perror("seteuid");
        exit(EXIT_FAILURE);
    }
    
    printf("\nNew EUID: %u  EGID: %u", geteuid(), getegid());

    fd = open(argv[1], O_RDWR | O_APPEND);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    } else {
        printf("\nFile opened: %d", fd);
    }

    bw = write(fd, buf, strlen(buf));
    if (bw == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    printf("\n%d bytes written into the file", bw);
    exit(EXIT_SUCCESS);

}