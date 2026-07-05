#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {

    gid_t saved_gid = getegid();

    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());

    setegid(getgid());

    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());

    setegid(saved_gid);

    printf("\nReal GID: %u", getgid());
    printf("\nEffective GID: %u", getegid());
  

    exit(EXIT_SUCCESS);
}