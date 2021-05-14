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

#define MY_INCOME_BANDWIDTH 10000

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

    int nfds = listenFD + MY_INCOME_BANDWIDTH / 2;
    int maxFDIndex = listenFD + 1;

    while (serverRunning) {

        if (maxFDIndex > nfds)
            nfds = maxFDIndex;
        if (nfds > listenFD + MY_INCOME_BANDWIDTH)
            nfds = listenFD + MY_INCOME_BANDWIDTH;

        memcpy(&workingSet, &masterSet, sizeof(masterSet));

        int readyDescriptorsNumber = select(nfds, &workingSet, nullptr, nullptr, nullptr);
        if (readyDescriptorsNumber < 0) {
            perror("  select() failed");
            exit(EXIT_FAILURE);
        }

        for (int i = listenFD; i <= listenFD + readyDescriptorsNumber; ++i) {
            if (FD_ISSET(i, &workingSet)) {

                if (i == listenFD) {
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
                } else {
                    bool cd = false;
                    readFromClient(i, serverRunning, cd);
                    if (cd) {
                        FD_CLR(i, &masterSet);
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
    std::cout << "server shutdown..." << std::endl;

    return 0;
}
