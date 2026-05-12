#pragma once
#include <string>

// ==========================================================
// Conexión Gopher individual
//   1. Leer selector del cliente
//   2. Convertir selector a ruta del filesystem
//   3a. Si es directorio → enviar menú Gopher
//   3b. Si es fichero    → enviar contenido del fichero
//   4. Cerrar conexión
// ==========================================================
class GopherConnection {
public:
    GopherConnection(int fd, const std::string& root,
                     const std::string& host, int port);
    void handle();

private:
    std::string read_selector();               // leer hasta \r\n
    std::string selector_to_path(const std::string& selector); // selector → ruta FS

    void send_file(const std::string& path);       // enviar fichero tal cual
    void send_directory(const std::string& path,   // enviar menú Gopher
                        const std::string& selector);

    int fd_;
    std::string root_;
    std::string host_;
    int port_;
};
