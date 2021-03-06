//
// Created by goforbroke on 04.05.2021.
//

#ifndef SERVER_SAMPLES_UTILS_H
#define SERVER_SAMPLES_UTILS_H

#if defined (WIN32)
static inline int poll( struct pollfd *pfd, int nfds, int timeout) { return WSAPoll ( pfd, nfds, timeout ); }
#endif

#include <cstring>

std::string getAppName(char **argv) {
    std::string tmp(argv[0]);
    size_t found = tmp.find_last_of("/");
    return tmp.substr(found + 1);
}

const char *ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string &rtrim(std::string &s, const char *t = ws) {
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string &ltrim(std::string &s, const char *t = ws) {
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string &trim(std::string &s, const char *t = ws) {
    return ltrim(rtrim(s, t), t);
}


#include <netinet/in.h> // sockaddr_in{} socket()
#include <netdb.h>      // gethostbyname()
#include <sys/ioctl.h>  // FIONBIO
#include <unistd.h>     // close()
#include <fcntl.h>      // F_GETFL

bool makeSocketNonblocking1(int socketFD) {
    int on = 1;

    /*************************************************************/
    /* Set socket to be nonblocking. All of the sockets for      */
    /* the incoming connections will also be nonblocking since   */
    /* they will inherit that state from the listening socket.   */
    /*************************************************************/
    if (ioctl(socketFD, FIONBIO, (char *) &on) < 0) {
        std::cerr << "[E] ioctl failed" << std::endl;
        return false;
    }

    return true;
}

bool makeSocketNonblocking2(int socketFD) {
    int flags = fcntl(socketFD, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "[E] fcntl failed (F_GETFL)\n";
        return false;
    }

    flags |= O_NONBLOCK;
    int s = fcntl(socketFD, F_SETFL, flags);
    if (s == -1) {
        std::cerr << "[E] fcntl failed (F_SETFL)\n";
        return false;
    }

    return true;
}

bool makeSocketNonblocking(int socketFD) {
    return makeSocketNonblocking1(socketFD) && makeSocketNonblocking2(socketFD);
}

inline int createSocketNonBlocking(const std::string &host, int port) {
    int listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD < 0) {
        perror("  socket() failed");
        exit(EXIT_FAILURE);
    }

    /*************************************************************/
    /* Allow socket descriptor to be reuseable                   */
    /*************************************************************/
    int on = 1;
    if (setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on)) < 0) {
        perror(" setsockopt() failed");
        exit(EXIT_FAILURE);
    }

    if (!makeSocketNonblocking(listenFD)) {
        perror(" makeSocketNonblocking() failed");
        exit(EXIT_FAILURE);
    }

    /*
     * Prepare sock addr and bind socket on it
     */
    struct sockaddr_in sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(port);

    if (host.empty()) {
        sockAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        auto he = gethostbyname(host.c_str());
        if (nullptr == he) {
            perror("  gethostbyname() failed");
            exit(EXIT_FAILURE);
        }
        memcpy(&sockAddr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    int bindRes = bind(listenFD, (struct sockaddr *) (&sockAddr), sizeof(sockAddr));
    if (bindRes != 0) {
        perror("  bind() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    int listenRes = listen(listenFD, 128);
    if (0 != listenRes) {
        perror("  listen() failed");
        close(listenFD);
        exit(EXIT_FAILURE);
    }

    return listenFD;
}

#define MAXMSG 2048

ssize_t readFromClient(int clientSockFD, bool &serverRunning, bool &disconnect) {
    char buffer[MAXMSG];
    memset(buffer, 0, sizeof buffer);

    std::string command;
    std::string respMsg = "pong\n";

    ssize_t nbytes = read(clientSockFD, buffer, MAXMSG);

    if (nbytes < 0) {
        return nbytes;
    }
    if (nbytes == 0) {
        close(clientSockFD);
        disconnect = true;
        return nbytes;
    }

    command = buffer;
    command = trim(command);
//    std::cout << "<<< " << command << std::endl;

    if (command == "shutdown")
        serverRunning = false;

    if (command == "exit") {
        disconnect = true;
        return 0;
    }

    send(clientSockFD, respMsg.c_str(), respMsg.length(), 0);
//    std::cout << ">>> " << respMsg << std::endl;

    return nbytes;
}

#endif //SERVER_SAMPLES_UTILS_H
