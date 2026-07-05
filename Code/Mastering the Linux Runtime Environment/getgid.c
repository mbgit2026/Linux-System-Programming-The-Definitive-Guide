#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <unistd.h>

int main() {

    gid_t rgid, egid;

    printf("\nReal group ID: %d", getgid());
    printf("\nEffective group ID: %d", getegid());

    exit(EXIT_SUCCESS);


}