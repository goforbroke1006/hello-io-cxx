//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()

#include "../utils.h"
#include "../SocketServerMetrics.h"

int main(int argc, char **argv) {

    SocketServerMetrics ssm(get_metric_name("hello-io-cxx"), get_metric_name("01-ss-1th"));
    ssm.init();

    auto appName = getAppName(argv);

    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 12000" << std::endl;
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

    int listenFD = socket(AF_INET, SOCK_STREAM, 0);


    if (bind(listenFD, (struct sockaddr *) (&sockAddr), sizeof(sockAddr)) != 0) {
        perror("bind() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }


    if (listen(listenFD, 128) != 0) {
        perror("listen() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    int clientSock;
    struct sockaddr_in clientSockAddr;
    int c = sizeof(struct sockaddr_in);
    std::string message = "pong\n";
    for (;;) {
        clientSock = accept(listenFD, (struct sockaddr *) &clientSockAddr, (socklen_t *) &c);
        if (clientSock < 0) {
            perror("accept() failed");
            continue;
        }

        std::cout << "client connected..." << std::endl;

        char income[2048];
        std::string command;

        for (;;) {

            memset(income, 0, sizeof(income));
            ssize_t readLen = read(clientSock, income, sizeof income);
            if (readLen < 0) {
                std::cerr << "can't read client message" << std::endl;
                continue;
            }
            command = income;
            command = trim(command);

            std::cout << "<<< " << command << std::endl;

            if (command == "exit" || command == "shutdown")
                break;

            send(clientSock, message.c_str(), message.length(), 0);
            std::cout << ">>> " << message << std::endl;
        }

        std::cout << "client disconnected..." << std::endl;

        close(clientSock);

        if (command == "shutdown")
            break;
    }

    close(listenFD);

    std::cout << "server shutdowning..." << std::endl;

    return 0;
}