#include "GeminiConnection.h"
#include "common.h"

#include <cstdio>
#include <cstring>
#include <cerrno>
#include <string>
#include <vector>
#include <algorithm>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

GeminiConnection::GeminiConnection(int fd, const std::string& root, SSL* ssl)
    : fd_(fd), root_(root), ssl_(ssl) {}

// handle() — punto de entrada para esta conexión
void GeminiConnection::handle() {
    std::string url      = read_url();
    std::string url_path = parse_url_path(url);

    // Validar la URL
    if (url_path.empty() || !is_safe_path(url_path)) {
        send_status(59, "Bad Request");
        return;
    }

    std::string fs_path = root_ + url_path;
    // Normalizar: quitar barra final excepto en "/"
    if (fs_path.size() > root_.size() + 1 && fs_path.back() == '/')
        fs_path.pop_back();

    struct stat st;
    if (stat(fs_path.c_str(), &st) < 0) {
        send_status(51, "Not Found");
        return;
    }

    if (S_ISDIR(st.st_mode))
        send_directory(fs_path, url_path);
    else
        send_file(fs_path);
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 4 — read_url
//   Lee bytes hasta \r\n. Máximo 1024 bytes (spec Gemini).
// ════════════════════════════════════════════════════════════════════════
std::string GeminiConnection::read_url() {
    std::string buf;
    char c;
    while (buf.size() < 1024) {
        int n = net_recv(&c, 1);
        if (n <= 0) break;
        if (c == '\n') break;
        if (c != '\r') buf += c;
    }
    return buf;
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 4 — parse_url_path
//   De "gemini://localhost:1965/docs/file.gmi" extrae "/docs/file.gmi".
//   Pasos:
//     1. Verificar que empieza por "gemini://"
//     2. Saltar host (y puerto opcional)
//     3. Devolver lo que queda (la ruta)
// ════════════════════════════════════════════════════════════════════════
std::string GeminiConnection::parse_url_path(const std::string& url) {
    const std::string prefix = "gemini://";
    if (url.substr(0, prefix.size()) != prefix)
        return "";                          // URL inválida
 
    // Buscar la primera '/' después del host
    size_t host_start = prefix.size();
    size_t slash = url.find('/', host_start);
 
    if (slash == std::string::npos)
        return "/";                         // sin ruta → raíz
 
    std::string path = url.substr(slash);   // incluye la '/' inicial
    if (path.empty()) path = "/";
    return path;
}


// Helper — send_status: Envía una línea de estado Gemini:  "<código> <meta>\r\n"
void GeminiConnection::send_status(int code, const std::string& meta) {
    std::string line = std::to_string(code) + " " + meta + "\r\n";
    net_send(line.c_str(), line.size());
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 5 — send_file
//   1. Enviar línea de estado:  "20 <mime-type>\r\n"
//   2. Enviar contenido del fichero
// ════════════════════════════════════════════════════════════════════════
void GeminiConnection::send_file(const std::string& path) {
    int file_fd = open(path.c_str(), O_RDONLY);
    if (file_fd < 0) {
        send_status(40, "Temporary Failure");
        return;
    }
 
    // Determinar MIME por extensión
    std::string mime = mime_for(path);
 
    // Enviar cabecera de éxito
    send_status(20, mime);
 
    // Enviar contenido del fichero en bloques
    char buf[4096];
    ssize_t n;
    while ((n = read(file_fd, buf, sizeof(buf))) > 0)
        net_send(buf, n);
 
    close(file_fd);
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 5 — send_directory
//   Genera un listado en formato Gemtext:
//     # Directory Listing
//     => /ruta/fichero.txt fichero.txt
//     => /ruta/subdir/    subdir/
// ════════════════════════════════════════════════════════════════════════
void GeminiConnection::send_directory(const std::string& path,
                                      const std::string& url_path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        send_status(40, "Temporary Failure");
        return;
    }
 
    // Recoger entradas
    std::vector<std::string> dirs, files;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        std::string full = path + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) < 0) continue;
        if (S_ISDIR(st.st_mode))
            dirs.push_back(name);
        else
            files.push_back(name);
    }
    closedir(dir);
    std::sort(dirs.begin(),  dirs.end());
    std::sort(files.begin(), files.end());
 
    // Enviar cabecera
    send_status(20, "text/gemini");
 
    // Normalizar base de la ruta para construir enlaces
    std::string base = url_path;
    if (base.empty() || base.back() != '/') base += '/';
 
    // Construir cuerpo gemtext
    std::string body = "# Directory Listing\r\n\r\n";
 
    for (auto& d : dirs)
        body += "=> " + base + d + "/  " + d + "/\r\n";
 
    for (auto& f : files)
        body += "=> " + base + f + "  " + f + "\r\n";
 
    net_send(body.c_str(), body.size());
}

ssize_t GeminiConnection::net_recv(char* buf, size_t len) {
    if (ssl_) return SSL_read(ssl_, buf, len);
    return recv(fd_, buf, len, 0);
}

ssize_t GeminiConnection::net_send(const char* buf, size_t len) {
    if (ssl_) return SSL_write(ssl_, buf, len);
    return send(fd_, buf, len, 0);
}