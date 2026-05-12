#include <cstdio>
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <climits>      // PATH_MAX

#include <unistd.h>     // realpath
#include <sys/stat.h>   // stat

#include "GopherServer.h"
#include "GeminiServer.h"

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

// Comprueba si una ruta es un directorio válido (sustituye a fs::is_directory)
static bool is_directory(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

// Convierte a ruta absoluta (sustituye a fs::absolute)
static std::string absolute_path(const std::string& path) {
    char resolved[PATH_MAX];
    if (realpath(path.c_str(), resolved) == nullptr)
        return path;   // si falla devuelve la original
    return std::string(resolved);
}

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

    root = absolute_path(root);

    if (!is_directory(root)) {
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