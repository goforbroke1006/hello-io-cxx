//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()
#include <sys/select.h> // fd_set{}
#include <sys/ioctl.h>  // FIONBIO

#include "../utils.h"

bool serverRunning = true;

int main(int argc, char **argv) {

    auto appName = getAppName(argv);

    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    char *serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    int on = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    if (ioctl(serverSocket, FIONBIO, (char *) &on) < 0) {
        perror("ioctl");
        exit(EXIT_FAILURE);
    }

    /*
     * Prepare sock addr and bind socket on it
     */
    struct sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(serverPort);

    auto he = gethostbyname(serverHost);
    if (nullptr == he) {
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    memcpy(&sockAddr.sin_addr, he->h_addr_list[0], he->h_length);

    int bindRes = bind(serverSocket, (struct sockaddr *) (&sockAddr), sizeof(sockAddr));
    if (bindRes != 0) {
        perror("bind");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    int listenRes = listen(serverSocket, 32);
    if (0 != listenRes) {
        perror("listen");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }


//    fd_set active_fd_set, read_fd_set;
    fd_set master_set, working_set;
    int max_sd;

//    FD_ZERO (&active_fd_set);
//    max_sd = serverSocket;
//    FD_SET (serverSocket, &active_fd_set);

    FD_ZERO(&master_set);
    max_sd = serverSocket;
    FD_SET(serverSocket, &master_set);

    struct sockaddr_in clientname;

    while (serverRunning) {
        memcpy(&working_set, &master_set, sizeof(master_set));


        int selectRes = select(max_sd + 10000, &working_set, nullptr, nullptr, nullptr);
        if (selectRes < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }
//        std::cout << "select() result: " << selectRes << std::endl;

        for (int i = 0; i <= max_sd; ++i) {
            if (FD_ISSET(i, &working_set)) {

                if (i == serverSocket) {
                    size_t size = sizeof(clientname);
                    int clientSock = accept(serverSocket, (struct sockaddr *) &clientname, (socklen_t *) &size);
                    if (clientSock < 0) {
                        perror("accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(clientSock, &master_set);
                    if (clientSock > max_sd) {
                        max_sd = clientSock;
                    }
                } else {
                    if (readFromClient(i, serverRunning) == 0) {
                        close(i);
                        FD_CLR (i, &master_set);
                    }
                }

            }
        }

    }

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (int i = 0; i <= max_sd; ++i) {
        if (FD_ISSET(i, &master_set))
            close(i);
    }


    close(serverSocket);
    std::cout << "server shutdowning..." << std::endl;

    return 0;
}
