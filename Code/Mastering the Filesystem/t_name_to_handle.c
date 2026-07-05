#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int                 mount_id, fhsize, flags, dirfd;
    char                *pathname;
    struct file_handle  *fhp;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pathname = argv[1];
    fhsize = sizeof(*fhp);
    fhp = malloc(fhsize);
    if (fhp == NULL)
        err(EXIT_FAILURE, "malloc");

    dirfd = AT_FDCWD;           
    flags = 0;                  
    fhp->handle_bytes = 0;
    if (name_to_handle_at(dirfd, pathname, fhp,
                            &mount_id, flags) != -1
        || errno != EOVERFLOW)
    {
        fprintf(stderr, "Unexpected result from name_to_handle_at()\n");
        exit(EXIT_FAILURE);
    }

    
    fhsize = sizeof(*fhp) + fhp->handle_bytes;
    fhp = realloc(fhp, fhsize);         
    if (fhp == NULL)
        err(EXIT_FAILURE, "realloc");


    if (name_to_handle_at(dirfd, pathname, fhp, &mount_id, flags) == -1)
        err(EXIT_FAILURE, "name_to_handle_at");

    printf("%d\n", mount_id);
    printf("%u %d   ", fhp->handle_bytes, fhp->handle_type);
    for (size_t j = 0; j < fhp->handle_bytes; j++)
        printf(" %02x", fhp->f_handle[j]);
    printf("\n");

    exit(EXIT_SUCCESS);
}
