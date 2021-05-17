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

    int clientFD = socket(AF_INET, SOCK_STREAM, 0);
    if (clientFD < 0) {
        perror("socket creation");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(serverPort);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, serverHost, &serv_addr.sin_addr) <= 0) {
        perror("  inet_pton() failed");
        exit(EXIT_FAILURE);
    }

    if (connect(clientFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("  connect() failed");
        close(clientFD);
        exit(EXIT_FAILURE);
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

    bool hasErrors = false;

    for (const auto &msg : messages) {

        usleep(500 * 1000);

        ssize_t sendLen = send(clientFD, msg.c_str(), msg.length(), 0);
        if (sendLen < 0) {
            perror("  send() failed");
            hasErrors = true;
            break;
        }
        if (((size_t) sendLen) != msg.length()) {
            std::cerr << "  Unexpected sent message length: got " << sendLen << " want " << msg.length() << std::endl;
            hasErrors = true;
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        ssize_t recvLen = recv(clientFD, buffer, 1024, 0);

        if (recvLen < 0) {
            perror("  recv() failed");
            hasErrors = true;
            break;
        }
        if (recvLen == 0) {
            // communication finished
            close(clientFD);
            break;
        }

        response = buffer;
        response = trim(response);

        if ("pong" != response) {
            std::cerr << "Unexpected response: \"" << response << "\"" << std::endl;
            hasErrors = true;
            break;
        }
    }

    // finalize
    close(clientFD);

    // on errors
    if (hasErrors) {
        exit(EXIT_FAILURE);
    }

    // summary
    auto endSpan = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endSpan - startSpan).count();
    std::cout << "SPEND TIME: " << duration << " milliseconds" << std::endl;

    exit(EXIT_SUCCESS);
}