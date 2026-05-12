#ifndef GEMINICONNECTION_H
#define GEMINICONNECTION_H

#include <string>

const int MAX_URL_SIZE = 1024;  // Gemini spec: URLs must not exceed 1024 bytes

// Gemini status codes
namespace GeminiStatus {
    const int INPUT = 10;
    const int SUCCESS = 20;
    const int REDIRECT_TEMPORARY = 30;
    const int REDIRECT_PERMANENT = 31;
    const int TEMPORARY_FAILURE = 40;
    const int SERVER_UNAVAILABLE = 41;
    const int CGI_ERROR = 42;
    const int PROXY_ERROR = 43;
    const int SLOW_DOWN = 44;
    const int PERMANENT_FAILURE = 50;
    const int NOT_FOUND = 51;
    const int GONE = 52;
    const int PROXY_REQUEST_REFUSED = 53;
    const int BAD_REQUEST = 59;
    const int CLIENT_CERT_REQUIRED = 60;
    const int CERT_NOT_AUTHORIZED = 61;
    const int CERT_NOT_VALID = 62;
}

class GeminiConnection {
public:
    GeminiConnection(int socket, bool use_tls = false);
    ~GeminiConnection();

    // Handle a Gemini request
    void handle_request();

private:
    int socket_fd;
    bool use_tls;
    void *tls_context;  // For TLS implementation (optional)
    
    // Read the URL from the client
    std::string read_url();
    
    // Parse a Gemini URL and extract the path
    std::string parse_url_path(const std::string& url);
    
    // Send a response header
    void send_header(int status, const std::string& meta);
    
    // Send a file to the client
    void send_file(const std::string& path);
    
    // Send a directory listing in gemtext format
    void send_directory(const std::string& path);
    
    // Convert filesystem path to URL path for links
    std::string path_to_url(const std::string& path);
};

#endif
