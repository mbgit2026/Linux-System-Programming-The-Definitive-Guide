#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <username> <filepath> [--no-initgroups]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *username = argv[1];
    const char *filepath = argv[2];
    int use_initgroups = 1;

    if (argc == 4 && strcmp(argv[3], "--no-initgroups") == 0) {
        use_initgroups = 0;
    }

    struct passwd *pw = getpwnam(username);
    if (!pw) {
        perror("getpwnam");
        return EXIT_FAILURE;
    }

    printf("Switching to user: %s (UID: %d, GID: %d)\n", username, pw->pw_uid, pw->pw_gid);

    if (use_initgroups) {
        printf("Calling initgroups()...\n");
        if (initgroups(username, pw->pw_gid) != 0) {
            perror("initgroups");
            return EXIT_FAILURE;
        }
    } else {
        printf("Skipping initgroups()...\n");
    }

    if (setgid(pw->pw_gid) != 0) {
        perror("setgid");
        return EXIT_FAILURE;
    }

    if (setuid(pw->pw_uid) != 0) {
        perror("setuid");
        return EXIT_FAILURE;
    }

    printf("Trying to open file: %s\n", filepath);

    int fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("open");
        printf("Access failed.\n");
    } else {
        printf("Access successful! File opened.\n");
        close(fd);
    }

    return 0;
}
