#include <stdio.h>
#include <unistd.h>
#include <sys/acct.h>
#include <errno.h>
#include <string.h>

int main() {
    const char *acct_file = "/var/account/pacct";  // Change if needed

    if (acct(acct_file) == -1) {
        printf("Failed to enable process accounting\n");
        printf("Error: %s\n", strerror(errno));
        return 1;
    }

    printf("Process accounting enabled. File: %s\n", acct_file);
    return 0;
}