#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/xattr.h>

int main(int argc, char *argv[])
{
    char *buf, *key, *val;
    ssize_t buflen, keylen, vallen;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s path\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    buflen = listxattr(argv[1], NULL, 0);
    if (buflen == -1) {
        perror("listxattr");
        exit(EXIT_FAILURE);
    }
    if (buflen == 0) {
        printf("%s has no attributes.\n", argv[1]);
        exit(EXIT_SUCCESS);
    }

    buf = malloc(buflen);
    if (buf == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    buflen = listxattr(argv[1], buf, buflen);
    if (buflen == -1) {
        perror("listxattr");
        exit(EXIT_FAILURE);
    }

    key = buf;
    while (buflen > 0) {

        printf("%s: ", key);

        vallen = getxattr(argv[1], key, NULL, 0);
        if (vallen == -1)
            perror("getxattr");

        if (vallen > 0) {
            val = malloc(vallen + 1);
            if (val == NULL) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            vallen = getxattr(argv[1], key, val, vallen);
            if (vallen == -1) {
                perror("getxattr");
            } else {
                val[vallen] = 0;
                printf("%s", val);
            }
            free(val);
        } else if (vallen == 0) {
            printf("<no value>");
        }

        printf("\n");

        keylen = strlen(key) + 1;
        buflen -= keylen;
        key += keylen;
    }

    free(buf);
    exit(EXIT_SUCCESS);
}

 