#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main(void) {
    struct hostent *host;

    sethostent(0);  // 1 = keep open, 0 = close after each call

    while ((host = gethostent()) != NULL) {
        printf("Name: %s\n", host->h_name);

        for (char **alias = host->h_aliases; *alias != NULL; alias++) {
            printf("  Alias: %s\n", *alias);
        }

        for (int i = 0; host->h_addr_list[i] != NULL; i++) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, host->h_addr_list[i], ip, sizeof(ip));
            printf("  Address: %s\n", ip);
        }

        printf("\n");
    }

    endhostent();
    return 0;
}
