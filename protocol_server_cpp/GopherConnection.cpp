//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                   Gopher Protocol Connection Handler
//                   RFC 1436
// 
//****************************************************************************

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "GopherConnection.h"
#include <sys/socket.h>
#include "common.h"

GopherConnection::GopherConnection(int socket) : socket_fd(socket) {
}

GopherConnection::~GopherConnection() {
    close(socket_fd);
}

// TODO: Students must implement this function
// Read the selector string from the client
// The selector is terminated by CR+LF (\r\n)
// Maximum selector size is MAX_SELECTOR_SIZE
std::string GopherConnection::read_selector() {
    char buf[MAX_SELECTOR_SIZE + 2];
    int total = 0;

    // TODO: Read from socket_fd until \r\n is found
    while (total < MAX_SELECTOR_SIZE) {
        char c;
        int n = recv(socket_fd, &c, 1, 0);
        if (n <= 0) break;          // error or connection closed
        if (c == '\n') break;       // end of selector
        buf[total++] = c;
    }

    // TODO: Return the selector string (without \r\n)
    // Strip trailing \r if present
    if (total > 0 && buf[total - 1] == '\r')
        total--;

    // TODO: Handle errors and buffer overflow
    // If total == MAX_SELECTOR_SIZE the selector was truncated; return what we have
    return std::string(buf, total);
}

// TODO: Students must implement this function
// Convert a Gopher selector to a filesystem path
// For example: "/" -> ".", "/file.txt" -> "./file.txt"
// Must ensure the path is safe (no directory traversal)
std::string GopherConnection::selector_to_path(const std::string& selector) {
    // TODO: Validate selector using is_safe_path()
    if (!is_safe_path(selector))
        return "";

    // TODO: Handle empty selector (should map to current directory)
    if (selector.empty() || selector == "/")
        return ".";

    // TODO: Convert selector to filesystem path
    // Selector always starts with '/', prepend '.' to make it relative
    return "." + selector;
}

// TODO: Students must implement this function
// Send a file to the client
// The file content is sent as-is, followed by closing the connection
void GopherConnection::send_file(const std::string& path) {
    // TODO: Open the file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        send_error("File not found");
        return;
    }

    // TODO: Read file in chunks and send via socket
    char buf[4096];
    while (file.read(buf, sizeof(buf)) || file.gcount() > 0) {
        std::streamsize n = file.gcount();
        // TODO: Handle errors (file not found, read errors, etc.)
        if (send(socket_fd, buf, static_cast<size_t>(n), 0) < 0)
            break;
    }

    // TODO: Close file when done
    file.close();
}

// TODO: Students must implement this function
// Send a directory listing in Gopher menu format
// Format: <type><display_string><TAB><selector><TAB><host><TAB><port><CR><LF>
// Example: 0README.txt<TAB>/README.txt<TAB>localhost<TAB>7070<CR><LF>
void GopherConnection::send_directory(const std::string& path) {
    // Obtain server host and port from the accepted socket's local address
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    getsockname(socket_fd, reinterpret_cast<struct sockaddr *>(&addr), &len);
    int server_port = ntohs(addr.sin_port);

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strncpy(hostname, "localhost", sizeof(hostname));
    }
    std::string host = hostname;

    // TODO: Open directory using opendir()
    DIR *dir = opendir(path.c_str());
    if (!dir) {
        send_error("Cannot open directory");
        return;
    }

    // Base selector: "." → "", "./docs" → "/docs"
    std::string base = (path == ".") ? "" : path.substr(1);

    // TODO: Read directory entries using readdir()
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string full_path = path + "/" + name;
        std::string selector  = base + "/" + name;

        // TODO: For each entry — Determine Gopher type using get_gopher_type()
        bool is_dir  = is_directory(full_path);
        char type    = get_gopher_type(name, is_dir);

        // TODO: Format menu line: type + name + TAB + selector + TAB + host + TAB + port + CRLF
        std::string line = std::string(1, type) + name
                         + "\t" + selector
                         + "\t" + host
                         + "\t" + std::to_string(server_port)
                         + "\r\n";

        // TODO: Send line to client
        send(socket_fd, line.c_str(), line.length(), 0);
    }

    // TODO: Send terminating line: ".<CR><LF>"
    std::string terminator = ".\r\n";
    send(socket_fd, terminator.c_str(), terminator.length(), 0);

    // TODO: Close directory
    closedir(dir);
}

// Send an error message in Gopher format
// Error type is '3'
void GopherConnection::send_error(const std::string& message) {
    std::string error_line = "3" + message + "\terror\terror\t0\r\n";
    send(socket_fd, error_line.c_str(), error_line.length(), 0);
}

// Main request handler
void GopherConnection::handle_request() {
    // Read the selector from the client
    std::string selector = read_selector();
    
    // Validate the selector
    if (!is_safe_path(selector)) {
        send_error("Invalid selector");
        return;
    }
    
    // Convert selector to filesystem path
    std::string path = selector_to_path(selector);
    
    // Check if path exists and determine type
    if (is_directory(path)) {
        send_directory(path);
    } else if (is_regular_file(path)) {
        send_file(path);
    } else {
        send_error("File not found");
    }
}
