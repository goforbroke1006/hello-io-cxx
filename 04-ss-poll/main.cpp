//
// Created by goforbroke on 04.05.2021.
//

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>     // close()
#include <sys/poll.h>   // pollfd{}

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

    struct pollfd fds[10000];
    int nfds = 1, startListen = 10000;
//    int current_size = 0;

    memset(fds, 0, sizeof(fds));
    fds[0].fd = listenFD;
    fds[0].events = POLLIN;

    while (serverRunning) {

        if (nfds > 10000)
            nfds = 10000;

        if (nfds > startListen)
            startListen = nfds;

        int pollRes = poll(fds, startListen, 0);
//        int pollRes = poll(fds, nfds, 0);
        if (pollRes < 0) {
            perror("poll() failed");
            serverRunning = false;
        }

//        current_size = nfds;
//        for (int i = 0; i < current_size; i++) {
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
                printf("  Error! [%d]revents = %d\n", i, fds[i].revents);
                serverRunning = false;
                break;
            }

            if (fds[i].fd == listenFD) {
                int clientSock = 0;
                do {
                    clientSock = accept(listenFD, nullptr, nullptr);
                    if (clientSock < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("  accept() failed");
                            serverRunning = false;
                        }
                        break;
                    }
                    fds[nfds].fd = clientSock;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (clientSock != -1);
            } else {
                bool cd = false;
                ssize_t resLen = readFromClient(fds[i].fd, serverRunning, cd);
                if (cd) {
                    fds[i].fd = -1;
                }
                if (resLen < 0) {
                    if (errno != EWOULDBLOCK) {
                        std::cerr << "i=" << i << " fd=" << fds[i].fd << std::endl;
                        perror("  recv() failed");
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


    close(listenFD);
    std::cout << "server shutdown..." << std::endl;

    return 0;
}
