//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//                      
//                     2º de grado de Ingeniería Informática
//                       
//                   Gemini Protocol Connection Handler
// 
//****************************************************************************

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "GeminiConnection.h"
#include <sys/socket.h>
#include "common.h"

// Helpers that transparently use TLS or plain socket
static int ssl_send(void *tls, int fd, const char *buf, size_t len) {
    if (tls)
        return SSL_write(reinterpret_cast<SSL *>(tls), buf, static_cast<int>(len));
    return static_cast<int>(::send(fd, buf, len, 0));
}

static int ssl_recv(void *tls, int fd, char *buf, size_t len) {
    if (tls)
        return SSL_read(reinterpret_cast<SSL *>(tls), buf, static_cast<int>(len));
    return static_cast<int>(::recv(fd, buf, len, 0));
}

GeminiConnection::GeminiConnection(int socket, bool use_tls)
    : socket_fd(socket), use_tls(use_tls), tls_context(nullptr) {

    // TODO (OPTIONAL - ADVANCED): Initialize TLS context if use_tls is true
    // This requires OpenSSL library
    if (!use_tls) return;

    // Create a new SSL context using the server-side TLS method
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        std::cerr << "SSL_CTX_new failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        return;
    }

    // Load the server certificate (cert.pem) and private key (key.pem)
    // These files must exist in the working directory (generated with openssl req -x509 ...)
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "SSL_CTX_use_certificate_file failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        SSL_CTX_free(ctx);
        return;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "SSL_CTX_use_PrivateKey_file failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        SSL_CTX_free(ctx);
        return;
    }

    // Create an SSL object and associate it with the accepted socket
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "SSL_new failed" << std::endl;
        SSL_CTX_free(ctx);
        return;
    }
    SSL_set_fd(ssl, socket_fd);

    // Perform the TLS handshake (server side)
    if (SSL_accept(ssl) <= 0) {
        std::cerr << "SSL_accept failed: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return;
    }

    // Store the SSL object in tls_context; SSL_CTX is retrievable via SSL_get_SSL_CTX()
    tls_context = ssl;
}

GeminiConnection::~GeminiConnection() {
    // TODO (OPTIONAL - ADVANCED): Clean up TLS context if it was initialized
    if (tls_context) {
        SSL *ssl = reinterpret_cast<SSL *>(tls_context);
        // Retrieve the SSL_CTX before freeing ssl (it's reference-counted separately)
        SSL_CTX *ctx = SSL_get_SSL_CTX(ssl);
        // Send TLS close_notify alert and free the SSL object
        SSL_shutdown(ssl);
        SSL_free(ssl);
        // Free the SSL context (created in the constructor)
        SSL_CTX_free(ctx);
        tls_context = nullptr;
    }

    close(socket_fd);
}

// TODO: Students must implement this function
// Read the URL from the client
// Format: <URL><CR><LF>
// Maximum URL size is MAX_URL_SIZE (1024 bytes)
// Returns the URL string (without CR+LF)
std::string GeminiConnection::read_url() {
    std::string url;
    url.reserve(MAX_URL_SIZE + 2);
    int bytes_read = 0;
    const int MAX_TOTAL_READ = MAX_URL_SIZE * 4;  // límite de seguridad

    // Leer byte a byte hasta '\n' o cierre de conexión.
    // Seguimos consumiendo bytes aunque ya estemos sobre el límite para drenar el buffer del socket y evitar RST al cerrar.
    while (bytes_read < MAX_TOTAL_READ) {
        char c;
        int n = ssl_recv(tls_context, socket_fd, &c, 1);
        if (n <= 0) break;
        bytes_read++;
        if (c == '\n') break;
        // Almacenamos hasta MAX_URL_SIZE+1 caracteres
        if (url.length() <= MAX_URL_SIZE)
            url += c;
    }

    if (!url.empty() && url.back() == '\r')
        url.pop_back();

    return url;
}

// TODO: Students must implement this function
// Parse a Gemini URL and extract the path component
// Format: gemini://hostname[:port]/path
// Example: "gemini://localhost/index.gmi" -> "/index.gmi"
// Example: "gemini://example.com:1965/docs/file.txt" -> "/docs/file.txt"
std::string GeminiConnection::parse_url_path(const std::string& url) {
    const std::string prefix = "gemini://";

    // TODO: Check if URL starts with "gemini://"
    if (url.compare(0, prefix.length(), prefix) != 0)
        return "/";

    // TODO: Find the first "/" after the hostname
    size_t path_start = url.find('/', prefix.length());

    // TODO: If no path, return "/"
    if (path_start == std::string::npos)
        return "/";

    // TODO: Extract and return the path
    std::string path = url.substr(path_start);
    if (path.empty()) path = "/";

    // TODO: Validate the path using is_safe_path()
    if (!is_safe_path(path))
        return "";

    return path;
}

// Send a Gemini response header
// Format: <STATUS><SPACE><META><CR><LF>
// Example: "20 text/gemini\r\n"
void GeminiConnection::send_header(int status, const std::string& meta) {
    std::ostringstream header;
    header << status << " " << meta << "\r\n";
    std::string header_str = header.str();
    
    // TODO (OPTIONAL): If using TLS, send via TLS — handled transparently by ssl_send()
    ssl_send(tls_context, socket_fd, header_str.c_str(), header_str.length());
}

// TODO: Students must implement this function
// Send a file to the client with appropriate Gemini header
void GeminiConnection::send_file(const std::string& path) {
    // TODO: Open the file
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        send_header(GeminiStatus::NOT_FOUND, "File not found");
        return;
    }

    // TODO: Get MIME type using get_mime_type()
    std::string mime = get_mime_type(path);

    // TODO: Send header: "20 <mime_type>\r\n"
    send_header(GeminiStatus::SUCCESS, mime);

    // TODO: Read file in chunks and send via socket
    // Uses SSL_write when TLS is active, send otherwise
    char buf[4096];
    while (file.read(buf, sizeof(buf)) || file.gcount() > 0) {
        std::streamsize n = file.gcount();
        // TODO: Handle errors (file not found, read errors, etc.)
        if (ssl_send(tls_context, socket_fd, buf, static_cast<size_t>(n)) < 0)
            break;
    }

    // TODO: Close file when done
    file.close();
}

// TODO: Students must implement this function
// Send a directory listing in gemtext format
// Gemtext format for links: "=> <URL> <DISPLAY_TEXT>"
// Example: "=> /docs/file.txt File.txt"
void GeminiConnection::send_directory(const std::string& path) {
    // TODO: Send header: "20 text/gemini\r\n"
    send_header(GeminiStatus::SUCCESS, "text/gemini");

    // TODO: Send title: "# Directory listing\n\n"
    std::string title = "# Directory Listing\n\n";
    ssl_send(tls_context, socket_fd, title.c_str(), title.length());

    // TODO: Open directory using opendir()
    DIR *dir = opendir(path.c_str());
    if (!dir) return;

    // Base URL path: "." → "", "./docs" → "/docs"
    std::string base = (path == ".") ? "" : path.substr(1);

    // TODO: Read directory entries using readdir()
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        // TODO: For each entry — Skip "." and ".."
        if (name == "." || name == "..") continue;

        std::string full_path = path + "/" + name;
        bool is_dir = is_directory(full_path);

        // TODO: Add "/" suffix for directories
        std::string url_path = base + "/" + name + (is_dir ? "/" : "");
        std::string display  = name + (is_dir ? "/" : "");

        // TODO: Format as gemtext link: "=> /<path>/<name> <name>\n"
        std::string line = "=> " + url_path + " " + display + "\n";

        // TODO: Send line to client
        ssl_send(tls_context, socket_fd, line.c_str(), line.length());
    }

    // TODO: Close directory
    closedir(dir);
}

// Convert filesystem path to URL path
std::string GeminiConnection::path_to_url(const std::string& path) {
    // Simple implementation: just ensure it starts with /
    if (path.empty() || path[0] != '/') {
        return "/" + path;
    }
    return path;
}

// Main request handler
void GeminiConnection::handle_request() {
    std::string url = read_url();

    if (url.length() > MAX_URL_SIZE) {
        send_header(GeminiStatus::BAD_REQUEST, "URL too long");
        return;
    }

    std::string path = parse_url_path(url);

    if (path.empty() || !is_safe_path(path)) {
        send_header(GeminiStatus::BAD_REQUEST, "Invalid path");
        return;
    }

    std::string fs_path = "." + path;

    if (is_directory(fs_path)) {
        send_directory(fs_path);
    } else if (is_regular_file(fs_path)) {
        send_file(fs_path);
    } else {
        send_header(GeminiStatus::NOT_FOUND, "Not found");
    }
}

