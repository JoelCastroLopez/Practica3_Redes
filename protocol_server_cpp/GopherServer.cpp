//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                        Gopher Protocol Server
//                        RFC 1436
// 
//****************************************************************************

#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include "common.h"
#include "GopherServer.h"
#include "GopherConnection.h"

// Thread function for handling client connections
void *run_gopher_connection(void *c) {
    GopherConnection *connection = (GopherConnection *) c;
    connection->handle_request();
    delete connection;
    return nullptr;
}

GopherServer::GopherServer(int port) : ProtocolServer(port) {
}

GopherServer::~GopherServer() {
    stop();
}

void GopherServer::run() {
    struct sockaddr_in client_addr;
    int client_socket;
    socklen_t addr_len = sizeof(client_addr);
    
    // Create and bind the socket
    std::pair<int, int> result = define_socket_TCP(port);
    msock = result.first;
    port = result.second;

    if (msock < 0) {
        errexit("Failed to create socket\n");
    }

    std::cout << "Gopher server listening on port " << port << std::endl;

    // Accept loop
    while (!should_stop) {
        client_socket = accept(msock, (struct sockaddr *) &client_addr, &addr_len);
        
        if (client_socket < 0) {
            if (should_stop) break;
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }
        
        // Create a new connection handler
        auto *connection = new GopherConnection(client_socket);
        
        // Create a thread to handle this connection
        pthread_t thread;
        pthread_create(&thread, nullptr, run_gopher_connection, (void *) connection);
        pthread_detach(thread);  // Detach so thread cleans up automatically
    }
}
