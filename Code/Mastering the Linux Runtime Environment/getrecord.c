#include <errno.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct passwd pwd;
    struct passwd *result;
    char *buf;
    long bufsize;
    int s;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s username\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1)          
        bufsize = 16384;        

    buf = malloc(bufsize);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    s = getpwnam_r(argv[1], &pwd, buf, bufsize, &result);
    if (result == NULL) {
        if (s == 0)
            printf("Not found\n");
        else {
            errno = s;
            perror("getpwnam_r");
        }
        exit(EXIT_FAILURE);
    }

    printf("User: %s \n", pwd.pw_name);
    printf("Password: %s \n", pwd.pw_passwd);
    printf("UID: %d \n", (intmax_t) pwd.pw_uid);
    printf("GID: %d \n", (intmax_t) pwd.pw_gid);
    printf("Comment: %s \n", pwd.pw_gecos);
    printf("Home: %s \n", pwd.pw_dir);
    printf("Shell: %s \n", pwd.pw_shell);

    exit(EXIT_SUCCESS);
}
