#pragma once
#include <string>
#include <openssl/ssl.h>
#include <sys/types.h>

class GeminiConnection {
public:
    GeminiConnection(int fd, const std::string& root, SSL* ssl = nullptr);
    void handle();

private:
    // NIVEL 4
    std::string read_url();
    std::string parse_url_path(const std::string& url);

    // NIVEL 5
    void send_file(const std::string& path);
    void send_directory(const std::string& path,
                        const std::string& url_path);

    // Helper de estado
    void send_status(int code, const std::string& meta);

    // Helpers de red (abstraen recv/send para TLS y sin TLS)
    ssize_t net_recv(char* buf, size_t len);
    ssize_t net_send(const char* buf, size_t len);

    int         fd_;
    std::string root_;
    SSL*        ssl_;   // null si no hay TLS
};