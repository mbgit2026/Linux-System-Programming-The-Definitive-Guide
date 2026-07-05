#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <protocol_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int proto_num = atoi(argv[1]);

    struct protoent *proto = getprotobynumber(proto_num);
    if (proto == NULL) {
        printf("❌ Protocol number %d not found.\n", proto_num);
        exit(EXIT_FAILURE);
    }

    printf("✅ Protocol number %d:\n", proto_num);
    printf("  Name  : %s\n", proto->p_name);
    for (char **alias = proto->p_aliases; *alias != NULL; alias++) {
        printf("  Alias : %s\n", *alias);
    }

    exit(EXIT_SUCCESS);
}
