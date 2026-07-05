#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

void print_net (struct netent *nt) {
  
    printf("\n✅ Network %s found \n", nt->n_name);
    printf("  Official Name: %s\n", nt->n_name);
    printf("  Address      :  %s\n", inet_ntoa((struct in_addr){htonl(nt->n_net)}));
  
    for (char **alias = nt->n_aliases; *alias != NULL; alias++) {
        printf("  Alias        : %s\n", *alias);
    }
}

void print_err (const char *name) {
  
    printf("❌ Network %s not found \n", name);
    exit(EXIT_FAILURE);

}

void lookup_by_name(const char *name) {
   
    struct netent *net = getnetbyname(name);
   
    if (net == NULL) {
        print_err(name);
    } else {
        print_net(net);
    }   
}

void lookup_by_addr(const char *addr_str) {
    
    struct in_addr addr;
    
    if (inet_aton(addr_str, &addr) == 0) {
        fprintf(stderr, "❌ Invalid IP address: %s\n", addr_str);
        return;
    }

    uint32_t net = ntohl(addr.s_addr); 
    struct netent *net_info = getnetbyaddr(net, AF_INET);
    
    if (net_info == NULL) {
        print_err(addr_str);
    } else {
        print_net(net_info);
    }
}

int main(int argc, char *argv[]) {
   
    if (argc != 3) {
        printf("Usage: %s -name <network_name>\n", argv[0]);
        printf("       %s -addr <network_address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-name") == 0) {
        lookup_by_name(argv[2]);
    } else if (strcmp(argv[1], "-addr") == 0) {
        lookup_by_addr(argv[2]);
    } else {
        fprintf(stderr, "❌ Unknown option: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
