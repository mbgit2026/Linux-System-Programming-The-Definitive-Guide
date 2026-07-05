#define _GNU_SOURCE
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


static int open_mount_path_by_id(int mount_id)
{
    int      mi_mount_id, found;
    char     mount_path[PATH_MAX];
    char     *linep;
    FILE     *fp;
    size_t   lsize;
    ssize_t  nread;

    fp = fopen("/proc/self/mountinfo", "r");
    if (fp == NULL)
        err(EXIT_FAILURE, "fopen");

    found = 0;
    linep = NULL;
    while (!found) {
        nread = getline(&linep, &lsize, fp);
        if (nread == -1)
            break;

        nread = sscanf(linep, "%d %*d %*s %*s %s",
                        &mi_mount_id, mount_path);
        if (nread != 2) {
            fprintf(stderr, "Bad sscanf()\n");
            exit(EXIT_FAILURE);
        }

        if (mi_mount_id == mount_id)
            found = 1;
    }
    free(linep);
    fclose(fp);

    if (!found) {
        fprintf(stderr, "Could not find mount point\n");
        exit(EXIT_FAILURE);
    }

    return open(mount_path, O_RDONLY);
}

int main(int argc, char *argv[])
{
    int                 mount_id, fd, mount_fd, handle_bytes;
    char                buf[1000];

#define LINE_SIZE 100

    char                line1[LINE_SIZE], line2[LINE_SIZE];
    char                *nextp;
    ssize_t             nread;
    struct file_handle  *fhp;

    if ((argc > 1 && strcmp(argv[1], "--help") == 0) || argc > 2) {
        fprintf(stderr, "Usage: %s [mount-path]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (fgets(line1, sizeof(line1), stdin) == NULL ||
        fgets(line2, sizeof(line2), stdin) == NULL)
    {
        fprintf(stderr, "Missing mount_id / file handle\n");
        exit(EXIT_FAILURE);
    }

    mount_id = atoi(line1);
    handle_bytes = strtoul(line2, &nextp, 0);

    fhp = malloc(sizeof(*fhp) + handle_bytes);
    if (fhp == NULL)
        err(EXIT_FAILURE, "malloc");

    fhp->handle_bytes = handle_bytes;

    fhp->handle_type = strtoul(nextp, &nextp, 0);

    for (size_t j = 0; j < fhp->handle_bytes; j++)
        fhp->f_handle[j] = strtoul(nextp, &nextp, 16);

    if (argc > 1)
        mount_fd = open(argv[1], O_RDONLY);
    else
        mount_fd = open_mount_path_by_id(mount_id);

    if (mount_fd == -1)
        err(EXIT_FAILURE, "opening mount fd");

    fd = open_by_handle_at(mount_fd, fhp, O_RDONLY);
    if (fd == -1)
        err(EXIT_FAILURE, "open_by_handle_at");

    nread = read(fd, buf, sizeof(buf));
    if (nread == -1)
        err(EXIT_FAILURE, "read");

    printf("Read %zd bytes\n", nread);
    printf("\nButes: %s", buf);

    exit(EXIT_SUCCESS);
}
