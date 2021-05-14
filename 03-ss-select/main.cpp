//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <cstring>
#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <unistd.h>     // close()
#include <sys/select.h> // fd_set{}


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

    int listenFD = openSocketNonBlocking(serverHost, serverPort);

    fd_set masterSet, workingSet;
    int max_sd;

    FD_ZERO(&masterSet);
    max_sd = listenFD;
    FD_SET(listenFD, &masterSet);

    struct sockaddr_in clientName;

    while (serverRunning) {

        memcpy(&workingSet, &masterSet, sizeof(masterSet));

        int selectRes = select(max_sd + 10000, &workingSet, nullptr, nullptr, nullptr);
        if (selectRes < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= max_sd; ++i) {
            if (FD_ISSET(i, &workingSet)) {

                if (i == listenFD) {
                    size_t size = sizeof(clientName);
                    int clientSock = accept(listenFD, (struct sockaddr *) &clientName, (socklen_t *) &size);
                    if (clientSock < 0) {
                        perror("accept");
                        break;
                    }
                    FD_SET(clientSock, &masterSet);
                    if (clientSock > max_sd) {
                        max_sd = clientSock;
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
    for (int i = 0; i <= max_sd; ++i) {
        if (FD_ISSET(i, &masterSet))
            close(i);
    }


    close(listenFD);
    std::cout << "server shutdowning..." << std::endl;

    return 0;
}
