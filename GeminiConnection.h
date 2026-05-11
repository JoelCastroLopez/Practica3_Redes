#pragma once
#include <string>
#include <openssl/ssl.h>

// ======================================================================
// Conexión Gemini individual
//   1. Leer URL del cliente (hasta \r\n, máx. 1024 bytes)
//   2. Extraer la ruta de la URL gemini://host/ruta
//   3. Construir ruta del filesystem
//   4a. Si es directorio → enviar listado gemtext  (20 text/gemini)
//   4b. Si es fichero    → enviar fichero con cabecera (20 mime/type)
//   4c. Si no existe     → enviar 51 Not Found
//   5. Cerrar conexión
//=======================================================================
class GeminiConnection {
public:
     GeminiConnection(int fd, const std::string& root, SSL* ssl = nullptr);
    void handle();

private:
    std::string read_url();                         // leer hasta \r\n (máx. 1024)
    std::string parse_url_path(const std::string& url); // extraer /ruta de la URL

    void send_file(const std::string& path);        // cabecera + contenido
    void send_directory(const std::string& path,    // cabecera + gemtext
                        const std::string& url_path);

    // Helpers para enviar respuestas Gemini
    void send_status(int code, const std::string& meta);

    int fd_;
    std::string root_;
    SSL* ssl_;
};
