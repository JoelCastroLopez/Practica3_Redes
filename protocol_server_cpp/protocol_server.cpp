//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                   Main entry point for protocol servers
// 
//****************************************************************************

#include <iostream>
#include <csignal>
#include <cstring>
#include "common.h"
#include "ProtocolServer.h"
#include "GopherServer.h"
#include "GeminiServer.h"

ProtocolServer *server = nullptr;

extern "C" void sighandler(int signal, siginfo_t *info, void *ptr) {
    std::cout << "Received signal, shutting down..." << std::endl;
    if (server) {
        server->stop();
    }
    exit(0);
}

void exit_handler() {
    if (server) {
        server->stop();
    }
}

void print_usage() {
    std::cout << "Usage: protocol_server <protocol> [port] [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Protocols:" << std::endl;
    std::cout << "  gopher    - Gopher protocol (RFC 1436)" << std::endl;
    std::cout << "  gemini    - Gemini protocol" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  port      - Port number (default: assigned by the Operating System)" << std::endl;
    std::cout << "  --tls     - Enable TLS for Gemini (optional, advanced feature)" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  protocol_server gopher" << std::endl;
    std::cout << "  protocol_server gopher 7070" << std::endl;
    std::cout << "  protocol_server gemini" << std::endl;
    std::cout << "  protocol_server gemini 1965" << std::endl;
    std::cout << "  protocol_server gemini 1965 --tls" << std::endl;
}

int main(int argc, char **argv) {
    // Set up signal handlers
    struct sigaction action{};
    action.sa_sigaction = sighandler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &action, nullptr);
    sigaction(SIGTERM, &action, nullptr);
    
    atexit(exit_handler);
    
    // Parse command line arguments
    if (argc < 2) {
        print_usage();
        errexit("\nError: Protocol not specified\n");
    }
    
    std::string protocol = argv[1];
    int port = 0;
    bool use_tls = false;
    
    // Parse port if provided
    if (argc >= 3 && strcmp(argv[2], "--tls") != 0) {
        if (sscanf(argv[2], "%i", &port) != 1) {
            print_usage();
            errexit("\nError: Port must be an integer\n");
        }
    }
    
    // Parse TLS flag for Gemini
    if (argc >= 3) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--tls") == 0) {
                use_tls = true;
            }
        }
    }
    
    // Create the appropriate server
    if (protocol == "gopher") {
        server = new GopherServer(port);
    } else if (protocol == "gemini") {
        server = new GeminiServer(port, use_tls);
    } else {
        print_usage();
        errexit("\nError: Unknown protocol '%s'\n", protocol.c_str());
    }
    
    // Run the server
    server->run();
    
    return 0;
}
