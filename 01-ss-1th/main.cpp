//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()

#include "utils.h"

int main(int argc, char **argv) {

    auto appName = getAppName(argv);

    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    char *serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    struct sockaddr_in sockAddr;
    sockAddr.sin_port = htons(serverPort);
    sockAddr.sin_family = AF_INET;

    auto he = gethostbyname(serverHost);
    if (nullptr == he) {
        std::cerr << "can't parse host name " << serverHost << std::endl;
        exit(EXIT_FAILURE);
    }
    memcpy(&sockAddr.sin_addr, he->h_addr_list[0], he->h_length);
    // sockAddr.sin_addr.s_addr = INADDR_ANY;

    int serv_socket = socket(AF_INET, SOCK_STREAM, 0);

    int bindRes = bind(serv_socket, (struct sockaddr *) (&sockAddr), sizeof(sockAddr));
    if (bindRes != 0) {
        std::cerr << "can't bind socket to address" << std::endl;
        exit(EXIT_FAILURE);
    }

    int listenRes = listen(serv_socket, 3);
    if (0 != listenRes) {
        std::cerr << "can't start socket listening" << std::endl;
        exit(EXIT_FAILURE);
    }

    int client_sock;
    struct sockaddr_in clientSockAddr;
    int c = sizeof(struct sockaddr_in);
    char *message = "pong\n";
    for (;;) {
        client_sock = accept(serv_socket, (struct sockaddr *) &clientSockAddr, (socklen_t *) &c);
        if (client_sock < 0) {
            std::cerr << "accept failed" << std::endl;
            continue;
        }

        std::cout << "client connected..." << std::endl;

        char income[2048];
        std::string command;

        for (;;) {

            memset(income, 0, sizeof income);
            size_t readLen = read(client_sock, income, sizeof income);
            if (readLen < 0) {
                std::cerr << "can't read client message" << std::endl;
                continue;
            }
            command = income;
            command = trim(command);

            std::cout << "<<< " << command << std::endl;

            if (command == "exit" || command == "shutdown")
                break;

            send(client_sock, message, strlen(message), 0);
            std::cout << ">>> " << message << std::endl;
        }

        std::cout << "client disconnected..." << std::endl;

        close(client_sock);

        if (command == "shutdown")
            break;
    }

    std::cout << "server shutdowning..." << std::endl;

    return 0;
}