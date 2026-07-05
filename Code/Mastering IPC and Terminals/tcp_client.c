#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char ip_str[INET_ADDRSTRLEN];

    if (argc < 4) {
        printf("Usage: %s <IP> <PORT> <MESSAGE>\n", argv[0]);
        return -1;
    }
    char *ip = argv[1];
    int port = atoi(argv[2]);
    char *message = argv[3];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation error");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IP address
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("invalid address");
        return -1;
    }

    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        return -1;
    }

    // Send message
    printf("send data to %s\n", inet_ntop(AF_INET, &serv_addr.sin_addr, ip_str, sizeof(ip_str)));
    send(sock, message, strlen(message), 0);
    printf("Sent: %s\n", message);

    // Receive response
    int n = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}