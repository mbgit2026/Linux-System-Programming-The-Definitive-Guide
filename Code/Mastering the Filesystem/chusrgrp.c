#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    uid_t uid;
    gid_t gid;

    if (argc != 4) {
        fprintf(stderr, "%s <owner> <group> <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uid = strtol(argv[1], NULL, 10);
    gid = strtol(argv[2], NULL, 10); 


    if (chown(argv[3], uid, gid) == -1) {
        perror("chown");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}