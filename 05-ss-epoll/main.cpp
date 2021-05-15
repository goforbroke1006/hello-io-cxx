//
// Created by goforbroke on 12.05.2021.
//

#include <iostream>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <unistd.h>     // close()
#include <sys/epoll.h>  // epoll_create()
#include <array>

#include "../utils.h"

#define MY_INCOME_BANDWIDTH 2500

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
        perror("  epoll_create() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.data.fd = listenFD;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, listenFD, &event) == -1) {
        perror("  epoll_ctl() failed");
        close(epollFD);
        close(listenFD);
        exit(EXIT_FAILURE);
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
                while (true) {
//                    struct sockaddr in_addr;
//                    socklen_t in_len = sizeof(in_addr);
                    clientSock = accept(listenFD, nullptr, nullptr);
                    if (clientSock == -1) {
                        if (errno != EWOULDBLOCK) {
                            perror("  accept() client failed");
                            serverRunning = false;
                        }
                        break;
                    }
                    if (!makeSocketNonblocking(clientSock)) {
                        perror("  makeSocketNonblocking() failed");
                        serverRunning = false;
                        break;
                    }
                    event.data.fd = clientSock;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, clientSock, &event) == -1) {
                        perror("  epoll_ctl() failed");
                        break;
                    }

                    std::cout << "[server] new client " << clientSock << std::endl;
                }
            } else {
                bool disconnect = false;
                readFromClient(events[i].data.fd, serverRunning, disconnect);
                if (disconnect) {
                    std::cout << "[server] close client " << events[i].data.fd << std::endl;
                    close(events[i].data.fd);
                }
            }
        }
    }

    for (const auto &e : events) {
        if (e.data.fd > 0)
            close(e.data.fd);
    }

    close(epollFD);
    close(listenFD);

    std::cout << "[server] shutdown..." << std::endl;

    return 0;
}