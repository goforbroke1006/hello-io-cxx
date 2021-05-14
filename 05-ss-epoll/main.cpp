//
// Created by goforbroke on 12.05.2021.
//

#include <iostream>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <unistd.h>     // close()
#include <sys/epoll.h>  // epoll_create()
#include <fcntl.h>
#include <array>

#include "../utils.h"

#define MY_INCOME_BANDWIDTH 10000

bool serverRunning = true;

int main(int argc, char **argv) {
    auto appName = getAppName(argv);
    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    int listenFD = createSocketNonBlocking(serverHost, serverPort);

    int epollFD = epoll_create1(0);
    if (epollFD == -1) {
        perror("epoll_create() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.data.fd = listenFD;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listenFD, &event) == -1) {
        std::cerr << "[E] epoll_ctl failed\n";
        return 1;

    }

    int clientSock;
    std::array<struct epoll_event, MY_INCOME_BANDWIDTH> events = {};

    while (serverRunning) {

        int ready = epoll_wait(epollFD, events.data(), events.size(), -1);
        if (ready == -1) {
            continue;
        }

        for (int i = 0; i < ready; i++) {

            if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN)) {
                std::cerr << "[E] epoll event error\n";
                close(events[i].data.fd);
                continue;
            }

            if (events[i].data.fd == listenFD) {
                do {
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof(in_addr);
                    clientSock = accept(listenFD, &in_addr, &in_len);
                    if (clientSock == -1) {
//                        perror("accept() client failed");
                        continue;
                    }
                    if (!makeSocketNonblocking(clientSock)) {
                        perror("make_socket_nonblocking() failed");
                        continue;
                    }
                    event.data.fd = clientSock;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSock, &event) == -1) {
                        perror("epoll_ctl() failed");
                        break;
                    }
                } while (clientSock != -1);
            } else {
                bool cd = false;
                readFromClient(events[i].data.fd, serverRunning, cd);
                if (cd) {
                    close(clientSock);
                }
            }
        }
    }

    close(listenFD);

    std::cout << "server shutdowning..." << std::endl;

    return 0;
}