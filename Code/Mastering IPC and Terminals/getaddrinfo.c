#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

void print_family(struct addrinfo *aip)
{
    printf(" family ");
    switch (aip->ai_family) {
    case AF_INET:
        printf("inet");
        break;
    case AF_INET6:
        printf("inet6");
        break;
#ifdef AF_UNIX
    case AF_UNIX:
        printf("unix");
        break;
#endif
    case AF_UNSPEC:
        printf("unspecified");
        break;
    default:
        printf("unknown");
    }
}

void print_type(struct addrinfo *aip)
{
    printf(" type ");
    switch (aip->ai_socktype) {
    case SOCK_STREAM:
        printf("stream");
        break;
    case SOCK_DGRAM:
        printf("datagram");
        break;
    case SOCK_SEQPACKET:
        printf("seqpacket");
        break;
    case SOCK_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)", aip->ai_socktype);
    }
}

void print_protocol(struct addrinfo *aip)
{
    printf(" protocol ");
    switch (aip->ai_protocol) {
    case 0:
        printf("default");
        break;
    case IPPROTO_TCP:
        printf("TCP");
        break;
    case IPPROTO_UDP:
        printf("UDP");
        break;
    case IPPROTO_RAW:
        printf("raw");
        break;
    default:
        printf("unknown (%d)", aip->ai_protocol);
    }
}

void print_flags(struct addrinfo *aip)
{
    printf(" flags");
    if (aip->ai_flags == 0) {
        printf(" 0");
    } else {
        if (aip->ai_flags & AI_PASSIVE)
            printf(" passive");
        if (aip->ai_flags & AI_CANONNAME)
            printf(" canon");
        if (aip->ai_flags & AI_NUMERICHOST)
            printf(" numhost");
        if (aip->ai_flags & AI_NUMERICSERV)
            printf(" numserv");
        if (aip->ai_flags & AI_V4MAPPED)
            printf(" v4mapped");
        if (aip->ai_flags & AI_ALL)
            printf(" all");
    }
}

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int err;

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    if (argc != 3) {
        fprintf(stderr, "usage: %s nodename service\n", argv[0]);
        exit(1);
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;

    if ((err = getaddrinfo(argv[1], argv[2], &hint, &ailist)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(err));
        exit(1);
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        print_flags(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);

        printf("\n\thost %s", aip->ai_canonname ? aip->ai_canonname : "-");

        if ((err = getnameinfo(aip->ai_addr, aip->ai_addrlen,
                               host, sizeof(host),
                               serv, sizeof(serv),
                               NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
            fprintf(stderr, "\n\tgetnameinfo error: %s\n", gai_strerror(err));
        } else {
            printf(" address %s", host);
            printf(" port %s", serv);
        }

        printf("\n");
    }

    freeaddrinfo(ailist);
    return 0;
}