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

#define INCOME_BANDWIDTH 10000

bool serverRunning = true;

auto make_socket_nonblocking(int socketfd) {
    int flags = fcntl(socketfd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "[E] fcntl failed (F_GETFL)\n";
        return false;
    }

    flags |= O_NONBLOCK;
    int s = fcntl(socketfd, F_SETFL, flags);
    if (s == -1) {
        std::cerr << "[E] fcntl failed (F_SETFL)\n";
        return false;
    }

    return true;
}

int main(int argc, char **argv) {
    auto appName = getAppName(argv);
    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD < 0) {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    if (!::make_socket_nonblocking(listenFD)) {
        perror("make_socket_nonblocking() failed");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    int yes = 1;
    if (setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) != 0) {
        perror("setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    if (bind(listenFD, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0) {
        perror("bind() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    if (listen(listenFD, 128) != 0) {
        perror("listen() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.data.fd = listenFD;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenFD, &event) == -1) {
        std::cerr << "[E] epoll_ctl failed\n";
        return 1;

    }

    int clientSock;
    struct sockaddr_storage clientsAddrs;
    socklen_t clientsAddrsSize = sizeof(clientsAddrs);

//    struct epoll_event events[INCOME_BANDWIDTH];
    std::array<struct epoll_event, INCOME_BANDWIDTH> events;

    while (serverRunning) {

        int ready = epoll_wait(epollfd, events.data(), events.size(), -1);
        if (ready == -1) {
//            perror("epoll_wait");
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
                    if (!make_socket_nonblocking(clientSock)) {
                        perror("make_socket_nonblocking() failed");
                        continue;
                    }
                    event.data.fd = clientSock;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSock, &event) == -1) {
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