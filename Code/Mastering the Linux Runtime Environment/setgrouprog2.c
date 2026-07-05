#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>


int openfile(char *filename) {
    int fd;

    fd = open(filename, O_RDONLY);
    if (fd < 0)  {
        perror("open");
        return EXIT_FAILURE;
    } else {
        close(fd);
        printf("\nFile opened successfully\n");
        return EXIT_SUCCESS;
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage %s <filename>", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];

    gid_t saved_gid = getegid();
    uid_t saved_uid = geteuid();

    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());
    openfile(filename);

    printf("\nDropping privileges...");
    setegid(getgid());
    seteuid(getuid());
    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());
    printf("\nReal UID: %u", getuid());
    printf("\nEffective UID: %u", geteuid());
    openfile(filename);

    printf("\n\nRegain privileges...");
    setegid(saved_gid);
    seteuid(saved_uid);

    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());
    printf("\nReal UID: %u", getuid());
    printf("\nEffective UID: %u", geteuid());
    openfile(filename);

    exit(EXIT_SUCCESS);
}

