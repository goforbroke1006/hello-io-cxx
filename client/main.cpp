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

#include "../utils.h"

#define BUFFER_SIZE 1024

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
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverHost, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
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

    char buffer[BUFFER_SIZE];

    auto startSpan = std::chrono::high_resolution_clock::now();
    std::string response;

    for (const auto &msg : messages) {
        ssize_t sendLen = send(sockfd, msg.c_str(), msg.length(), 0);
        if (sendLen < 0) {
            perror("  send() failed");
            exit(EXIT_FAILURE);
        }
        if (((size_t) sendLen) != msg.length()) {
            std::cerr << "  Unexpected sent message length: got " << sendLen << " want " << msg.length() << std::endl;
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, sizeof(buffer));
        ssize_t recvLen = recv(sockfd, buffer, 1024, 0);

        if (recvLen < 0) {
            perror("  recv() failed");
            exit(EXIT_FAILURE);
        }
        if (recvLen == 0) {
            // communication finished
            close(sockfd);
            break;
        }

        response = buffer;
        response = trim(response);

        if ("pong" != response) {
            std::cerr << "Unexpected response: \"" << response << "\"" << std::endl;
            exit(EXIT_FAILURE);
        }
//        std::cout << ">>> (" << valread << "): " << buffer << std::endl;
    }

    auto endSpan = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endSpan - startSpan).count();
    std::cout << "SPEND TIME: " << duration << " milliseconds" << std::endl;

    close(sockfd);
    return 0;
}