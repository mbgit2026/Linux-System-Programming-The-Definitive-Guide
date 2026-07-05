#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <gid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    gid_t gid = atoi(argv[1]);
    int match;
    
    match = group_member(gid);

    if (match != 0) 
        printf("\nGID %u group found: %s", gid, getgrgid(gid)->gr_name);
    else 
        printf("\nNo matching group found");

    exit(EXIT_SUCCESS);
}