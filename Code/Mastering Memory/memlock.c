#include <unistd.h>
#include <stdio.h>
#include <sys/resource.h>

int main() {


    printf("\n%d", sysconf(_SC_MEMLOCK_RANGE));
    printf("\n%d", sysconf(_SC_MEMLOCK));
    
    struct rlimit lim;

    if (getrlimit(RLIMIT_MEMLOCK, &lim) == -1) {
        perror("getrlimit");
        return 1;
    }

    printf("\nRLIMIT_MEMLOCK (soft limit): ");

    if (lim.rlim_cur == RLIM_INFINITY)
        printf("unlimited\n");
    else
        printf("%lu bytes\n", (unsigned long)lim.rlim_cur);

    printf("RLIMIT_MEMLOCK (hard limit): ");

    if (lim.rlim_max == RLIM_INFINITY)
        printf("unlimited\n");
    else
        printf("%lu bytes\n", (unsigned long)lim.rlim_max);

    return 0;
}


