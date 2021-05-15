//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>     // close()
#include <sys/select.h> // fd_set{} FD_ZERO() FD_SET() select() FD_ISSET() FD_CLR()

#include "../utils.h"

bool serverRunning = true;

#define MY_INCOME_BANDWIDTH 2500

int main(int argc, char **argv) {

    auto appName = getAppName(argv);

    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 8080" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    int listenFD = createSocketNonBlocking(serverHost, serverPort);

    fd_set masterSet, workingSet;

    FD_ZERO(&masterSet);
    FD_SET(listenFD, &masterSet);

    struct sockaddr_in clientName;

    const int nfds = listenFD + MY_INCOME_BANDWIDTH;
    int maxFDIndex = listenFD + 1;

    while (serverRunning) {

        FD_ZERO(&workingSet);
        memcpy(&workingSet, &masterSet, sizeof(masterSet));

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;

        int readyDescriptorsNumber = select(nfds, &workingSet, nullptr, nullptr, &timeout);
        if (readyDescriptorsNumber < 0) {
            perror("  select() failed");
            continue;
        }

        for (int clientFD = listenFD; clientFD <= listenFD + readyDescriptorsNumber; ++clientFD) {
            if (FD_ISSET(clientFD, &workingSet)) {

                if (clientFD == listenFD) {
                    size_t size = sizeof(clientName);
                    int clientSock = accept(listenFD, (struct sockaddr *) &clientName, (socklen_t *) &size);
                    if (clientSock < 0) {
                        perror("  accept() failed");
                        break;
                    }
                    FD_SET(clientSock, &masterSet);
                    if (clientSock > maxFDIndex) {
                        maxFDIndex = clientSock;
                    }
                    std::cout << "[server] new client " << clientSock << std::endl;
                } else if (clientFD == -1) {
                    continue;
                } else {
                    bool disconnect = false;
                    ssize_t resLen = readFromClient(clientFD, serverRunning, disconnect);
                    if (disconnect) {
                        std::cout << "[server] close client " << clientFD << std::endl;
                        close(clientFD);
                        FD_CLR(clientFD, &masterSet);
                        FD_CLR(clientFD, &workingSet);
                    }
                    if (resLen < 0) {
                        if (errno != EWOULDBLOCK) {
                            std::cerr << "i=" << clientFD << " fd=" << clientFD << std::endl;
                            perror("  recv() failed");
                            serverRunning = false;
                            break;
                        }
                    }
                }

            }
        }

    }

    /*************************************************************/
    /* Clean up all of the sockets that are open                 */
    /*************************************************************/
    for (int i = 0; i <= nfds; ++i) {
        if (FD_ISSET(i, &masterSet))
            close(i);
    }


    close(listenFD);
    std::cout << "[server] shutdown..." << std::endl;

    return 0;
}
