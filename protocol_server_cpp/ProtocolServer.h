#ifndef PROTOCOLSERVER_H
#define PROTOCOLSERVER_H

#include <string>

// Define a TCP socket and bind it to a port
// If port is 0, a random available port will be assigned
// Returns the socket descriptor, or -1 on error
std::pair<int, int> define_socket_TCP(int port);

// Base class for protocol servers
class ProtocolServer {
public:
    ProtocolServer(int port = 0);
    virtual ~ProtocolServer();

    // Start the server (blocking call)
    virtual void run() = 0;

    // Stop the server
    virtual void stop();

    // Get the port the server is listening on
    int get_port() const { return port; }

protected:
    int port;
    int msock;  // Main socket descriptor
    bool should_stop;
};

#endif
