//
// Created by goforbroke on 04.05.2021.
//
// Based on this sample: https://www.geeksforgeeks.org/socket-programming-cc/
//

#include <iostream>     // cin cout cerr endl
#include <arpa/inet.h>  // sockaddr_in{}
#include <sys/socket.h> // connect()
#include <cstring>      // strlen()
#include <vector>
#include <unistd.h>     // close(fd)
#include <chrono>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./client 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    char *serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    struct sockaddr_in serv_addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "socket creation error" << std::endl;
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverHost, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    std::vector<std::string> messages = {
            "hi",
            "hello",
            "guten tag",
            "ping",
            "wake up",
            "how r u",
            "have a nice day",
            "bye-bye",
            "see you",
            "exit",
    };

    char buffer[1024] = {0};

    auto startSpan = std::chrono::high_resolution_clock::now();

    for (const auto &msg : messages) {
        const char *text = msg.c_str();
        send(sockfd, text, strlen(text), 0);
//        std::cout << "<<< " << text << std::endl;

        memset(buffer, 0, sizeof buffer);
        size_t valread = recv(sockfd, buffer, 1024, 0);
//        std::cout << ">>> (" << valread << "): " << buffer << std::endl;
    }

    auto endSpan = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endSpan - startSpan).count();
    std::cout << "SPEND TIME: " << duration << " milliseconds" << std::endl;

    close(sockfd);
    return 0;
}