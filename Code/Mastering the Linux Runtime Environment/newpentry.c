#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 9) {
        fprintf(stderr, "Usage: %s <output_file> user passwd uid gid gecos home shell\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *outfile = fopen(argv[1], "w");
    if (outfile == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    struct passwd pw;

    pw.pw_name = argv[2];
    pw.pw_passwd = argv[3];           
    pw.pw_uid = (uid_t) atoi(argv[4]);
    pw.pw_gid = (gid_t) atoi(argv[5]);
    pw.pw_gecos = argv[6];
    pw.pw_dir = argv[7];
    pw.pw_shell = argv[8];

    if (putpwent(&pw, outfile) != 0) {
        perror("putpwent");
        fclose(outfile);
        return EXIT_FAILURE;
    }

    fclose(outfile);
    printf("User entry written to %s\n", argv[1]);
    return EXIT_SUCCESS;
}
