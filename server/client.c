#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9000
#define SERVER_IP "172.17.0.1" // Change to the server's IP address

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[1024] = {0};
    char message[] = "Hello from client";

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send a message to the server
    send(sockfd, message, strlen(message), 0);
    printf("Hello message sent\n");

    // Receive a response from the server
    read(sockfd, buffer, sizeof(buffer));
    printf("Message from server: %s\n", buffer);

    // Close the socket
    close(sockfd);

    return 0;
}