#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]) {

    int res;
    char *confname;

    if (argc != 3) {
        printf ("\nUsage: %s <file> <variable>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((res = pathconf(argv[1], atoi(argv[2]))) == -1) { 
        perror("pathconf");
        exit(EXIT_FAILURE);
    }

    switch (atoi(argv[2]))
    {
    case 0:
        confname = "_PC_LINK_MAX";
        break;
    case 1:
        confname = "_PC_MAX_CANON";
        break;
    case 2:
        confname = "_PC_MAX_INPUT";
        break;
    case 3:
        confname = "_PC_NAME_MAX";
        break;
    case 4:
        confname = "_PC_PATH_MAX";
        break;
    case 5:
        confname = "_PC_PIPE_BUF";
        break;
    case 6: 
        confname = "_PC_CHOWN_RESTRICTED";
        break;
    case 7:
        confname = "_PC_NO_TRUNC";
        break;
    default:
        confname = "Option not found";
        break;
    }


    printf("\n%s: %d", confname, res);

    exit(EXIT_SUCCESS);
}