#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    ssize_t nread, nwritten, tot;
    char buf[BUF_SIZE];
    FILE *fp;
    int corefd;
    char cwd[PATH_MAX];

    /* Change to crashing process working directory */
    snprintf(cwd, PATH_MAX, "/proc/%s/cwd", argv[1]);
    chdir(cwd);

    /* Open metadata file */
    fp = fopen("core.info", "w");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    /* Open core dump output file */
    corefd = open("core.dump",
                  O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR);
    if (corefd == -1)
        exit(EXIT_FAILURE);

    /* Log arguments */
    fprintf(fp, "argc=%d\n", argc);
    for (size_t j = 0; j < argc; j++)
        fprintf(fp, "argv[%zu]=<%s>\n", j, argv[j]);

    /* Copy core dump from stdin to file */
    tot = 0;
    while ((nread = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
        char *p = buf;
        ssize_t remaining = nread;

        while (remaining > 0) {
            nwritten = write(corefd, p, remaining);
            if (nwritten <= 0)
                exit(EXIT_FAILURE);

            remaining -= nwritten;
            p += nwritten;
        }
        tot += nread;
    }

    fprintf(fp, "Total bytes in core dump: %zd\n", tot);

    close(corefd);
    fclose(fp);
    exit(EXIT_SUCCESS);
}
