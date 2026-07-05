#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    printf("Helper running\n");
    printf("UID  = %d\n", getuid());
    printf("EUID = %d\n", geteuid());

    FILE *f = fopen("/etc/shadow", "r");
    if (!f) {
        perror("fopen shadow");
        return 1;
    }

    printf("Successfully opened /etc/shadow\n");
    fclose(f);

    return 0;
}
