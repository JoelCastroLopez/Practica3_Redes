#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <filesystem>

#include "GopherServer.h"
#include "GeminiServer.h"

namespace fs = std::filesystem;

// Comprobación de argumentos correctos
static void usage(const char* prog) {
    fprintf(stderr,
        "Uso:\n"
        "  %s gopher [puerto] [directorio]\n"
        "  %s gemini [puerto] [directorio]\n"
        "\n"
        "Valores por defecto:\n"
        "  puerto     → 7070 (gopher) / 1965 (gemini)\n"
        "  directorio → ./demo/gopher_demo o ./demo/gemini_demo\n",
        prog, prog);
    exit(1);
}

// Función main, que también se encarga de cortar la línea de argumentos y hacer funcionar el código con el protocolo seleccionado
int main(int argc, char* argv[]) {
    if (argc < 2) usage(argv[0]);

    std::string proto = argv[1];
    int port = 0;
    std::string root;

    if (proto == "gopher") {
        port = (argc >= 3) ? atoi(argv[2]) : 7070;
        root = (argc >= 4) ? argv[3] : "demo/gopher_demo";
    } else if (proto == "gemini") {
        port = (argc >= 3) ? atoi(argv[2]) : 1965;
        root = (argc >= 4) ? argv[3] : "demo/gemini_demo";
    } else {
        usage(argv[0]);
    }

    // Convertir a ruta absoluta
    root = fs::absolute(root).string();

    if (!fs::is_directory(root)) {
        fprintf(stderr, "ERROR: '%s' no es un directorio valido\n", root.c_str());
        return 1;
    }

    try {
        if (proto == "gopher") {
            GopherServer server(port, root);
            server.run();
        } else {
            GeminiServer server(port, root);
            server.run();
        }
    } catch (const std::exception& e) {
        fprintf(stderr, "ERROR: %s\n", e.what());
        return 1;
    }

    return 0;
}
