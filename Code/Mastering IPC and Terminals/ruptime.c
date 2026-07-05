#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <err.h>

#define BUFLEN 128
#define MAXSLEEP 128

int connect_retry(int domain, int type, int protocol, const struct sockaddr *addr, socklen_t alen)
{
    int numsec, fd;
    /* Try to connect with exponential backoff. */

    for (numsec = 1; numsec <= MAXSLEEP; numsec <<= 1) {
        if ((fd = socket(domain, type, protocol)) < 0)
            return(-1);
        if (connect(fd, addr, alen) == 0) {
        /* * Connection accepted. */
            return(fd);
        }
        close(fd);

        /* * Delay before trying again. */
        if (numsec <= MAXSLEEP/2)
            sleep(numsec);
    }
    return(-1);
}


void print_uptime(int sockfd)
{
    int n;
    char buf[BUFLEN];
    
    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
        write(STDOUT_FILENO, buf, n);
    
    if (n < 0)
        err(EXIT_FAILURE, "recv error");
   
}

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, lst;

    if (argc != 2)
        err(EXIT_FAILURE, "usage: ruptime hostname");

    memset(&hint, 0, sizeof(hint));
    hint.ai_socktype = SOCK_STREAM;
    
    if ((lst = getaddrinfo(argv[1], "12345", &hint, &ailist)) != 0)
        err(EXIT_FAILURE, "getaddrinfo error: %s", gai_strerror(lst));

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0) {
            continue;
        } else {
            print_uptime(sockfd);
            close(sockfd);
            freeaddrinfo(ailist);
            exit(0);
        }
    }
    err(EXIT_FAILURE, "can’t connect to %s", argv[1]);
}