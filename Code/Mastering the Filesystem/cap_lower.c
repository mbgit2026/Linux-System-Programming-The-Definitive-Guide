#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/capability.h>

int main() {
    // Step 1: Enable CAP_NET_BIND_SERVICE dynamically
    cap_t caps = cap_get_proc();
    if (!caps) {
        perror("cap_get_proc");
        return 1;
    }

    cap_value_t cap_list[] = { CAP_NET_BIND_SERVICE };
    if (cap_set_flag(caps, CAP_EFFECTIVE, 1, cap_list, CAP_SET) == -1) {
        perror("cap_set_flag");
        cap_free(caps);
        return 1;
    }

    if (cap_set_proc(caps) == -1) {
        perror("cap_set_proc");
        cap_free(caps);
        return 1;
    }

    cap_free(caps);
    printf("CAP_NET_BIND_SERVICE enabled.\n");

    // Step 2: Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    // Step 3: Prepare server address on port 80
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80); // Privileged port
    addr.sin_addr.s_addr = INADDR_ANY;

    // Step 4: Bind to port 80
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        return 1;
    }

    printf("Bound to port 80 successfully.\n");
    close(sockfd);
    return 0;
}
