//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()
#include <thread>
#include <mutex>

#include "../utils.h"

std::mutex acceptConnectionsMX;

bool serverRunning = true;

void process_client(int listenFD) {
    int clientSock;
    struct sockaddr_in clientSockAddr;
    int c = sizeof(struct sockaddr_in);
    char *message = "pong\n";

    while (serverRunning) {

        acceptConnectionsMX.lock();
        clientSock = accept(listenFD, (struct sockaddr *) &clientSockAddr, (socklen_t *) &c);
        acceptConnectionsMX.unlock();

        if (clientSock < 0) {
            perror("accept() failed");
            continue;
        }

        std::cout << "client connected..." << std::endl;

        char income[2048];
        std::string command;

        for (;;) {

            memset(income, 0, sizeof income);
            size_t readLen = read(clientSock, income, sizeof income);
            if (readLen == 0) {
                close(clientSock);
                continue;
            }
            if (readLen < 0) {
                std::cerr << "can't read client message" << std::endl;
                continue;
            }
            command = income;
            command = trim(command);

            std::cout << "<<< " << command << std::endl;

            if (command == "exit") {
                close(clientSock);
                break;
            }
            if (command == "shutdown") {
                serverRunning = false;
                break;
            }

            send(clientSock, message, strlen(message), 0);
            std::cout << ">>> " << message << std::endl;
        }

        std::cout << "client disconnected..." << std::endl;

        close(clientSock);

        if (command == "shutdown")
            break;
    }
}

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

    int listenFD = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(listenFD, (struct sockaddr *) (&sockAddr), sizeof(sockAddr)) != 0) {
        std::cerr << "can't bind socket to address" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(listenFD, 128) != 0) {
        std::cerr << "can't start socket listening" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::thread th1(process_client, listenFD);
    std::thread th2(process_client, listenFD);
    std::thread th3(process_client, listenFD);
    std::thread th4(process_client, listenFD);
    std::thread th5(process_client, listenFD);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();

    close(listenFD);
    std::cout << "server shutdowning..." << std::endl;

    return 0;
}