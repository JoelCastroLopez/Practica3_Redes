#ifndef GOPHERCONNECTION_H
#define GOPHERCONNECTION_H

#include <string>

const int MAX_SELECTOR_SIZE = 1024;

class GopherConnection {
public:
    GopherConnection(int socket);
    ~GopherConnection();

    // Handle a Gopher request
    void handle_request();

private:
    int socket_fd;
    int port;
    
    // Parse the selector from the client
    std::string read_selector();
    
    // Send a file to the client
    void send_file(const std::string& path);
    
    // Send a directory listing (Gopher menu)
    void send_directory(const std::string& path);
    
    // Send an error message
    void send_error(const std::string& message);
    
    // Convert selector to filesystem path
    std::string selector_to_path(const std::string& selector);
};

#endif
