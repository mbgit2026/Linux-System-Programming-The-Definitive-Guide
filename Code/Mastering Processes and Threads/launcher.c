#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>

int main(int argc, char *argv[]) {

    printf("Launcher UID=%d EUID=%d\n", getuid(), geteuid());

    if (argc > 1 && strcmp(argv[1], "nnp") == 0) {
        printf("Setting PR_SET_NO_NEW_PRIVS\n");

        if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0,0,0) == -1) {
            perror("prctl NO_NEW_PRIVS");
            exit(1);
        }
    }

    printf("Exec helper...\n");

    execl("./helper", "./helper", NULL);
    perror("execl");
    return 1;
}
