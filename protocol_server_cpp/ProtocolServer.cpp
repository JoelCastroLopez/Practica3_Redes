//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                   Base class for protocol servers
// 
//****************************************************************************

#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "common.h"
#include "ProtocolServer.h"

// TODO: Students must implement this function
// This function should create a TCP socket, bind it to the specified port,
// and start listening for connections.
// If port is 0, the OS will assign a random available port.
// The function should return the socket descriptor.
std::pair<int, int> define_socket_TCP(int port) {
    // TODO: Create socket using socket()
    // TODO: Bind socket to port using bind()
    // TODO: Start listening using listen()
    // TODO: If port was 0, retrieve the assigned port using getsockname()
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        errexit("socket: %s\n", strerror(errno));

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port        = htons(port);

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)) < 0)
        errexit("bind: %s\n", strerror(errno));

    if (listen(sock, SOMAXCONN) < 0)
        errexit("listen: %s\n", strerror(errno));

    if (port == 0) {
        socklen_t len = sizeof(sin);
        if (getsockname(sock, reinterpret_cast<struct sockaddr *>(&sin), &len) < 0)
            errexit("getsockname: %s\n", strerror(errno));
        port = ntohs(sin.sin_port);
    }
    
    return std::pair<int, int>(sock, port);  // Replace with actual socket descriptor and port number
}

ProtocolServer::ProtocolServer(int port) : port(port), msock(-1), should_stop(false) {
}

ProtocolServer::~ProtocolServer() {
    stop();
}

void ProtocolServer::stop() {
    should_stop = true;
    if (msock >= 0) {
        shutdown(msock, SHUT_RDWR);
        close(msock);
        msock = -1;
    }
}
