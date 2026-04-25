#include "GopherConnection.h"
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

GopherConnection::GopherConnection(int fd, const std::string& root,
                                   const std::string& host, int port)
    : fd_(fd), root_(root), host_(host), port_(port) {}

// handle() — punto de entrada para esta conexión
void GopherConnection::handle() {
    std::string selector = read_selector();
    std::string path     = selector_to_path(selector);

    // Evitamos path traversal
    if (!is_safe_path(selector)) {
        // Enviar mensaje de error Gopher (tipo '3')
        std::string err = "3Error: ruta no permitida\t/\t" + host_ +
                          "\t" + std::to_string(port_) + "\r\n.\r\n";
        send(fd_, err.c_str(), err.size(), 0);
        return;
    }

    struct stat st;
    if (stat(path.c_str(), &st) < 0) {
        std::string err = "3No encontrado: " + selector + "\t/\t" +
                          host_ + "\t" + std::to_string(port_) + "\r\n.\r\n";
        send(fd_, err.c_str(), err.size(), 0);
        return;
    }

    if (S_ISDIR(st.st_mode))
        send_directory(path, selector);
    else
        send_file(path);
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 2 — read_selector
//   Lee bytes del socket hasta encontrar \r\n (o \n sólo).
//   Límite: 1024 bytes.
// ════════════════════════════════════════════════════════════════════════
std::string GopherConnection::read_selector() {
    std::string buf;
    char c;
    while (buf.size() < 1024) {
        int n = recv(fd_, &c, 1, 0);
        if (n <= 0) break;
        if (c == '\n') break;       // fin de línea
        if (c != '\r') buf += c;    // ignorar \r
    }
    if (buf.empty()) buf = "/";     // selector vacío = raíz
    return buf;
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 2 — selector_to_path
//   Convierte el selector Gopher (ej. "/docs/readme.txt")
//   en ruta absoluta del filesystem (ej. "/var/gopher/docs/readme.txt").
//   Si el selector es "/" o vacío → devuelve root_.
// ════════════════════════════════════════════════════════════════════════
std::string GopherConnection::selector_to_path(const std::string& selector) {
    if (selector == "/" || selector.empty())
        return root_;

    // Asegurar que empieza por /
    std::string sel = selector;
    if (sel[0] != '/') sel = "/" + sel;

    return root_ + sel;
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 3 — send_file
//   Abre el fichero y lo envía byte a byte por el socket.
//   No añade terminador: el cierre de conexión indica fin.
// ════════════════════════════════════════════════════════════════════════
void GopherConnection::send_file(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        std::string err = "3No se puede abrir el fichero\t/\t" + host_ +
                          "\t" + std::to_string(port_) + "\r\n.\r\n";
        send(fd_, err.c_str(), err.size(), 0);
        return;
    }

    char buf[4096];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0)
        send(fd_, buf, n, 0);

    close(fd);
}

// ════════════════════════════════════════════════════════════════════════
// NIVEL 3 — send_directory
//   Lee el directorio y genera un menú Gopher.
//
//   Formato de cada línea:
//     <tipo><texto_visible>\t<selector>\t<host>\t<puerto>\r\n
//
//   Al final siempre:
//     .\r\n
// ════════════════════════════════════════════════════════════════════════
void GopherConnection::send_directory(const std::string& path,
                                      const std::string& sel) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::string err = "3No se puede abrir el directorio\t/\t" +
                          host_ + "\t" + std::to_string(port_) + "\r\n.\r\n";
        send(fd_, err.c_str(), err.size(), 0);
        return;
    }

    // Recoger entradas y ordenarlas: directorios primero, luego ficheros
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

    std::string menu;

    // Directorios
    for (auto& d : dirs) {
        std::string item_sel = (sel == "/" ? "" : sel) + "/" + d;
        menu += "1" + d + "\t" + item_sel + "\t" + host_ +
                "\t" + std::to_string(port_) + "\r\n";
    }

    // Ficheros
    for (auto& f : files) {
        char type = gopher_type_for(f);
        std::string item_sel = (sel == "/" ? "" : sel) + "/" + f;
        menu += type + f + "\t" + item_sel + "\t" + host_ +
                "\t" + std::to_string(port_) + "\r\n";
    }

    // Terminador obligatorio del menú Gopher
    menu += ".\r\n";

    send(fd_, menu.c_str(), menu.size(), 0);
}
