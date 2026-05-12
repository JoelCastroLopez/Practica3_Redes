//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                        Gemini Protocol Server
//                        gemini://gemini.circumlunar.space/docs/specification.gmi
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
#include "GeminiServer.h"
#include "GeminiConnection.h"

// Thread function for handling client connections
void *run_gemini_connection(void *c) {
    GeminiConnection *connection = (GeminiConnection *) c;
    connection->handle_request();
    delete connection;
    return nullptr;
}

GeminiServer::GeminiServer(int port, bool use_tls) 
    : ProtocolServer(port), use_tls(use_tls) {
}

GeminiServer::~GeminiServer() {
    stop();
}

void GeminiServer::run() {
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

    std::cout << "Gemini server listening";

    if (use_tls) {
        std::cout << " (TLS enabled)";
    } else {
        std::cout << " (TLS disabled - for testing only)";
    }
    std::cout << " port = " << port << std::endl;

    // Accept loop
    while (!should_stop) {
        client_socket = accept(msock, (struct sockaddr *) &client_addr, &addr_len);
        
        if (client_socket < 0) {
            if (should_stop) break;
            std::cerr << "Accept failed: " << strerror(errno) << std::endl;
            continue;
        }
        
        // Create a new connection handler
        auto *connection = new GeminiConnection(client_socket, use_tls);
        
        // Create a thread to handle this connection
        pthread_t thread;
        pthread_create(&thread, nullptr, run_gemini_connection, (void *) connection);
        pthread_detach(thread);  // Detach so thread cleans up automatically
    }
}
