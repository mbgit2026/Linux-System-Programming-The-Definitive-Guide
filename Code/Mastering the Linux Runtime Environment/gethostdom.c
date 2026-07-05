#define _GNU_SOURCE  
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>

int main(void) {
    char hostname[HOST_NAME_MAX + 1];
    char domainname[HOST_NAME_MAX + 1];

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }

    if (getdomainname(domainname, sizeof(domainname)) == -1) {
        perror("getdomainname");
        return 1;
    }

    printf("Hostname    : %s\n", hostname);
    printf("Domain name : %s\n", domainname);

    if (sethostname("linux", 5) == -1) {
        perror("sethostname");
        return 1;
    }
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }
    printf("Hostname    : %s\n", hostname);


    return 0;
}
