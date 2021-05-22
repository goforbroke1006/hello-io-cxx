//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>     // close()
#include <sys/select.h> // fd_set{} FD_ZERO() FD_SET() select() FD_ISSET() FD_CLR()

#include "../utils.h"
#include "../SocketServerMetrics.h"

bool serverRunning = true;

//#define MY_INCOME_BANDWIDTH 2500

int main(int argc, char **argv) {
    SocketServerMetrics ssm("serversample", "03ssselect");
    ssm.init();

    auto appName = getAppName(argv);

    if (argc != 3) {
        std::cerr << "Usage: " << appName << " 0.0.0.0 12000" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string serverHost = argv[1];
    int serverPort = atoi(argv[2]);

    int listenFD = createSocketNonBlocking(serverHost, serverPort);

    fd_set masterSet, workingSet;

    FD_ZERO(&masterSet);
    FD_SET(listenFD, &masterSet);

//    struct sockaddr_in clientName;

    const int nfds = FD_SETSIZE; //listenFD + MY_INCOME_BANDWIDTH;
    int maxFDIndex = listenFD + 1;

    while (serverRunning) {

        FD_ZERO(&workingSet);
        memcpy(&workingSet, &masterSet, sizeof(masterSet));

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int readyDescriptorsNumber = select(nfds, &workingSet, nullptr, nullptr, &timeout);
        if (readyDescriptorsNumber == 0) {
            // timeout exceeded, anyone event happens
            continue;
        }
        if (readyDescriptorsNumber < 0) {
            perror("  select() failed");
            serverRunning = false;
            break;
        }

        for (int clientFD = listenFD; clientFD <= listenFD + nfds; ++clientFD) {
            if (FD_ISSET(clientFD, &workingSet)) {

                if (clientFD == listenFD) {
                    while (true) {
                        int clientSock = accept(listenFD, nullptr, nullptr);
                        if (clientSock < 0) {
                            break; // no new clients
                        }
                        FD_SET(clientSock, &masterSet);
                        if (clientSock > maxFDIndex) {
                            maxFDIndex = clientSock;
                        }
                        std::cout << "[server] new client " << clientSock << std::endl;
                        ssm.acceptedClients().Increment();
                    }
                } else if (clientFD == -1) {
                    continue;
                } else {
                    bool disconnect = false;
                    ssize_t readLen = readFromClient(clientFD, serverRunning, disconnect);
                    if (disconnect) {
                        std::cout << "[server] close client " << clientFD << std::endl;
                        close(clientFD);
                        FD_CLR(clientFD, &masterSet);
                        FD_CLR(clientFD, &workingSet);
                    }
                    if (readLen < 0) {
                        if (errno != EWOULDBLOCK && errno != EBADF) {
                            std::cerr << "i=" << clientFD << " fd=" << clientFD << " errno=" << errno << std::endl;
                            perror("  recv() failed");
                            serverRunning = false;
                            break;
                        }
                    }
                    if (readLen >= 0){
                        ssm.processedMessages().Increment();
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
