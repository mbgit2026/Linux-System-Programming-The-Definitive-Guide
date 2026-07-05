#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

void print_limit(const char *name, int resource) {
    struct rlimit limit;

    if (getrlimit(resource, &limit) == -1) {
        perror("getrlimit");
        return;
    }

    printf("%s:\n", name);
    if (limit.rlim_cur == RLIM_INFINITY)
        printf("  Soft limit: unlimited \n");
    else
        printf("  Soft limit: %llu\n", (unsigned long long)limit.rlim_cur);
    
    if (limit.rlim_max == RLIM_INFINITY) 
        printf("  Hard limit: unlimited \n");
    else 
        printf("  Hard limit: %llu\n", (unsigned long long)limit.rlim_max);

    printf("\n");
}

int main() {
    print_limit("Max number of open files (RLIMIT_NOFILE)", RLIMIT_NOFILE);
    print_limit("Max virtual memory size (RLIMIT_AS)", RLIMIT_AS);
    print_limit("Max CPU time (RLIMIT_CPU)", RLIMIT_CPU);
    print_limit("Max Stack size (RLIMIT_STACK)", RLIMIT_STACK);

    return 0;
}
