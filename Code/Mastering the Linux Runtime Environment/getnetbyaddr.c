#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("\nUsage %s <net_addr>", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct in_addr addr;
    if (!inet_aton(argv[1], &addr)) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[1]);
        return 1;
    }

    uint32_t net_num = ntohl(addr.s_addr);
    struct netent *net = getnetbyaddr(net_num, AF_INET);

    if (net == NULL) {
        printf("\nNetwork %s not found", argv[1]);
        exit(EXIT_FAILURE);
    } else {
        printf("\n✅ Network %s found \n\n", net->n_name);
        printf("  Official Name: %s\n", net->n_name);
        printf("  Address      :  %s\n", inet_ntoa((struct in_addr){htonl(net->n_net)}));
        for (char **alias = net->n_aliases; *alias != NULL; alias++) {
            printf("  Alias        : %s\n", *alias);
        }
    }

    exit(EXIT_SUCCESS);
}