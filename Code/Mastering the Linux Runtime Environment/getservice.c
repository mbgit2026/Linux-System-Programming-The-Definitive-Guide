#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <service_name> <tcp|udp>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *service = argv[1];
    const char *protocol = argv[2];

    if (strcmp(protocol, "tcp") != 0 && strcmp(protocol, "udp") != 0) {
        fprintf(stderr, "❌ Protocol must be 'tcp' or 'udp'\n");
        return EXIT_FAILURE;
    }

    struct servent *svc = getservbyname(service, protocol);
    if (svc == NULL) {
        printf("❌ No service found for '%s/%s'\n", service, protocol);
        return EXIT_FAILURE;
    }

    printf("✅ Service: %s\n", svc->s_name);
    printf("  Port   : %d\n", ntohs(svc->s_port));
    printf("  Proto  : %s\n", svc->s_proto);

    for (char **alias = svc->s_aliases; *alias != NULL; alias++) {
        printf("  Alias  : %s\n", *alias);
    }

    return EXIT_SUCCESS;
}
