//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()
#include <sys/ioctl.h>  // FIONBIO
#include <sys/poll.h>   // pollfd{}

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


    struct pollfd fds[10000];
    int nfds = 1, startListen = 9000;
    int current_size = 0;

    memset(fds, 0, sizeof(fds));
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    while (serverRunning) {

        if (nfds > 10000)
            nfds = 10000;

        if (nfds > startListen)
            startListen = nfds;

        int pollRes = poll(fds, startListen, 0);
        if (pollRes < 0) {
            perror("select");
            serverRunning = false;
        }

//        current_size = nfds;
        for (int i = 0; i < startListen; i++) {

            /*********************************************************/
            /* Loop through to find the descriptors that returned    */
            /* POLLIN and determine whether it's the listening       */
            /* or the active connection.                             */
            /*********************************************************/
            if (fds[i].revents == 0)
                continue;

            /*********************************************************/
            /* If revents is not POLLIN, it's an unexpected result,  */
            /* log and end the server.                               */
            /*********************************************************/
            if (fds[i].revents != POLLIN) {
                printf("  Error! revents = %d\n", fds[i].revents);
                serverRunning = false;
                break;
            }

            if (fds[i].fd == serverSocket) {
                int clientSock = 0;
                do {
                    clientSock = accept(serverSocket, nullptr, nullptr);
                    if (clientSock < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("accept failed");
                            serverRunning = false;
                        }
                        break;
                    }
                    fds[nfds].fd = clientSock;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (clientSock != -1);
            } else {
                ssize_t resLen = readFromClient(fds[i].fd, serverRunning);
                if (resLen < 0) {
                    if (errno != EWOULDBLOCK) {
                        perror("recv() failed");
                        serverRunning = false;
                    }
                    break;
                }
            }


        }

    }

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (int i = 0; i <= nfds; ++i) {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }


    close(serverSocket);
    std::cout << "server shutdowning..." << std::endl;

    return 0;
}
